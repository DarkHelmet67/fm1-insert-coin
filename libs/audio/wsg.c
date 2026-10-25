/* SPDX-License-Identifier: GPL-3.0-only */
#include "wsg.h"

#include "voice.h"

/** The voice counters are 20 bits, kept here with 4 bits of fraction: 24 bits in all. */
#define COUNTER_MASK 0xffffffu

/** The top 5 bits of the counter pick the sample of the waveform. */
#define SAMPLE_SHIFT 19

/** Samples of one tick at 44.1 kHz: 120 ticks a second. */
#define TICK_SAMPLES ((AUDIO_RATE + 119) / 120)

/**
 * Loudness: three voices of samples from -8 to 7 at volume 15 give at most 360, which this gain
 * brings to about 70% of full scale, as loud as the other games' effects.
 */
#define WSG_GAIN 64

WsgFrame wsg_silent_frame(void)
{
    WsgFrame frame;
    for (int t = 0; t < WSG_TICKS; t++)
        for (int v = 0; v < WSG_VOICES; v++) {
            frame.tick[t][v].frequency = 0;
            frame.tick[t][v].volume = 0;
            frame.tick[t][v].waveform = 0;
        }
    return frame;
}

Wsg wsg_new(void)
{
    Wsg wsg = {{0, 0, 0}, 0};
    return wsg;
}

uint32_t wsg_step(uint32_t frequency)
{
    return ((frequency & 0xfffffu) * 2229u) >> 6;
}

int32_t wsg_sample(const uint8_t prom[WSG_PROM_SIZE], int32_t waveform, int32_t index)
{
    return (prom[(waveform & 7) * WSG_WAVE_LENGTH + (index & 31)] & 0x0f) - 8;
}

void wsg_render(Wsg *wsg, const WsgFrame *frame, const uint8_t prom[WSG_PROM_SIZE], int32_t *mix,
                uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        const WsgVoice *voices = frame->tick[wsg->position < TICK_SAMPLES ? 0 : 1];
        int32_t sum = 0;
        for (int v = 0; v < WSG_VOICES; v++) {
            wsg->counters[v] = (wsg->counters[v] + wsg_step(voices[v].frequency)) & COUNTER_MASK;
            if (voices[v].volume != 0)
                sum += wsg_sample(prom, voices[v].waveform,
                                  (int32_t)(wsg->counters[v] >> SAMPLE_SHIFT)) *
                       voices[v].volume;
        }
        mix[i] += sum * WSG_GAIN;
        if (wsg->position < 2u * TICK_SAMPLES)
            wsg->position++;
    }
}

void wsg_start_frame(Wsg *wsg)
{
    wsg->position = 0;
}
