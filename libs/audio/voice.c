/* SPDX-License-Identifier: GPL-3.0-only */
#include "voice.h"

#include "pow2.h"

/** The quietest volume at the end of an exponential fade, as SILENCE in sound.ts (0.0001). */
#define SILENCE_PER_MILLION 100

/** Full scale in Q15. */
#define Q15_ONE 32767

/**
 * The square wave's level: the web version's oscillators are band-limited and scaled so their
 * peaks, Gibbs ripples included, reach full scale, so the flat part of a square sits about
 * 1.5 dB lower (measured with tools/sounds/compare.py).
 */
#define SQUARE_LEVEL 27525

/** Samples in `ms` milliseconds. */
static int32_t samples(uint32_t ms)
{
    return (int32_t)(ms * (AUDIO_RATE / 1000u) + ms * (AUDIO_RATE % 1000u) / 1000u);
}

/** Phase step per sample for a pitch in hertz: 2^32 / 44100 = 97391.5 per hertz. */
static uint32_t increment_for(uint32_t hz)
{
    return hz * 97392u;
}

/**
 * The one-pole low-pass coefficient for a cut-off: 2 pi fc / (fs + 2 pi fc), in Q15. Two such
 * stages in a row fall as steeply as the web version's biquad (12 dB per octave); voice_start
 * places them at 1.5 times the cut-off, where two poles lose 3 dB at the cut-off as the biquad.
 */
static int32_t filter_for(uint32_t cutoff_hz)
{
    const uint32_t w = cutoff_hz * 6283u / 1000u;
    return (int32_t)((w << 15) / (AUDIO_RATE + w));
}

Voice voice_silent(void)
{
    Voice voice = {0};
    return voice;
}

Voice voice_start(const Sound *sound)
{
    Voice voice = voice_silent();
    voice.sound = sound;
    voice.position = -samples(sound->delay_ms);
    voice.length = samples(sound->duration_ms);
    voice.hold = samples(sound->hold_ms < sound->duration_ms ? sound->hold_ms : sound->duration_ms);
    voice.sweep_octaves_q16 = log2_q16(sound->to_hz) - log2_q16(sound->from_hz);
    /* the fade falls from the volume to SILENCE: log2(volume / 0.0001) octaves */
    voice.fade_octaves_q16 = log2_q16((uint32_t)sound->volume * 1000u / SILENCE_PER_MILLION);
    voice.filter_q15 = sound->cutoff_hz ? filter_for(sound->cutoff_hz * 2u) : 0;
    voice.increment = increment_for(sound->from_hz);
    voice.noise = 0x12345678u;
    return voice;
}

int voice_is_free(const Voice *voice)
{
    return voice->sound == 0;
}

/** Whole control blocks in `n` samples, at least one (so it can divide). */
static int32_t blocks_of(int32_t n)
{
    return n / CONTROL_SAMPLES > 0 ? n / CONTROL_SAMPLES : 1;
}

/*
 * Sweeps and fades advance block by block: products of a block count (at most a few thousand)
 * and a value in Q15 or Q16 stay within 32 bits, and no 64-bit division is needed.
 */

/** The pitch's phase step at the voice's position. */
static uint32_t current_increment(const Voice *voice)
{
    const Sound *s = voice->sound;
    const int32_t done = voice->position / CONTROL_SAMPLES;
    const int32_t total = blocks_of(voice->length);
    if (s->sweep == CURVE_LINEAR) {
        const int32_t hz = s->from_hz + ((int32_t)s->to_hz - s->from_hz) * done / total;
        return increment_for((uint32_t)(hz > 0 ? hz : 0));
    }
    /* even to the ear: a straight line in octaves, in sixteenths of hertz for precision */
    const int32_t octaves = voice->sweep_octaves_q16 / total * done;
    return increment_for(scale_pow2((uint32_t)s->from_hz * 16u, octaves) >> 4);
}

/** The volume at the voice's position: held, then falling to silence at the end. */
static int32_t current_gain(const Voice *voice)
{
    const Sound *s = voice->sound;
    const int32_t volume = (int32_t)s->volume * Q15_ONE / 1000;
    const int32_t into_fade = voice->position - voice->hold;
    const int32_t fade = voice->length - voice->hold;
    if (into_fade <= 0 || fade <= 0)
        return volume;
    const int32_t done = into_fade / CONTROL_SAMPLES;
    const int32_t total = blocks_of(fade);
    if (s->fade == CURVE_LINEAR)
        return done >= total ? 0 : volume * (total - done) / total;
    return (int32_t)scale_pow2((uint32_t)volume, -(voice->fade_octaves_q16 / total * done));
}

/** The raw wave (or noise) at the voice's phase, in Q15, and the generators moved on. */
static int32_t source_sample(Voice *voice)
{
    const Sound *s = voice->sound;
    if (s->kind == SOUND_NOISE) {
        /* xorshift: white noise, the same every time */
        uint32_t x = voice->noise;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        voice->noise = x;
        return (int32_t)(x >> 16) - 32768;
    }
    const uint32_t p = voice->phase;
    voice->phase += voice->increment;
    switch (s->wave) {
    case WAVE_SQUARE:
        return p < 0x80000000u ? SQUARE_LEVEL : -SQUARE_LEVEL;
    case WAVE_SAWTOOTH:
        return (int32_t)(p >> 16) - 32768;
    default: {
        const int32_t t = (int32_t)(p >> 15); /* 0 .. 131071 */
        return t < 65536 ? t - 32768 : 98303 - t;
    }
    }
}

/** The sample through the two low-pass stages, when the sound has a filter. */
static int32_t filtered(Voice *voice, int32_t x)
{
    if (!voice->filter_q15)
        return x;
    voice->low1 += (x - voice->low1) * voice->filter_q15 >> 15;
    voice->low2 += (voice->low1 - voice->low2) * voice->filter_q15 >> 15;
    return voice->low2;
}

void voice_render(Voice *voice, int32_t *mix, uint32_t n)
{
    for (uint32_t i = 0; i < n && voice->sound; i++) {
        if (voice->position < 0) {
            voice->position++;
            continue;
        }
        if (voice->position % CONTROL_SAMPLES == 0) {
            voice->increment = current_increment(voice);
            voice->gain_q15 = current_gain(voice);
        }
        const int32_t x = filtered(voice, source_sample(voice));
        mix[i] += x * voice->gain_q15 >> 15;
        if (++voice->position >= voice->length)
            *voice = voice_silent();
    }
}
