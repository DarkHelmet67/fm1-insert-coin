/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound on the FM-1: the main loop asks for effects with audio_play() and gives the voices
 * of a WSG chip with audio_voices(); the sound interrupt mixes them with audio_render(), called
 * from app_audio(). Include from main.c, after platform.c.
 *
 * The two sides meet in a small queue: only the main loop writes `audio_written`, only the
 * interrupt writes `audio_read`, so neither needs to switch the other off. The voices go through
 * two slots: the main loop fills the one the interrupt is not reading, then counts it posted.
 */
#ifndef AUDIO_FM1_H
#define AUDIO_FM1_H

#include "audio/mixer.h"
#include "audio/wsg.h"

/** Effects asked for and not yet started: more than a frame ever asks for. */
#define AUDIO_QUEUE 16u

/** The frames the platform asks for at once (platform.c: HALF_FRAMES). */
#define AUDIO_BLOCK 256u

static const SoundEffect *volatile audio_queue[AUDIO_QUEUE];
static volatile uint32_t audio_written, audio_read;
static Mixer audio_mixer;
static int32_t audio_mix[AUDIO_BLOCK];

/** The voices posted by the main loop: slot `audio_posted & 1` is the latest. */
static WsgFrame audio_slots[2];
static const uint8_t *volatile audio_prom;
static volatile uint32_t audio_posted;
/** The interrupt's own: the posts it has taken, the frame it plays and the chip playing it. */
static uint32_t audio_taken;
static WsgFrame audio_frame;
static Wsg audio_wsg;

/** Stops the compiler moving memory writes across this point (the slot before the count). */
#define AUDIO_BARRIER() __asm__ volatile("" ::: "memory")

/**
 * In the main loop, once a frame: what the WSG chip plays from now on, and its waveform PROM;
 * `frame` NULL (a game without the chip, the menu) is silence.
 */
static void audio_voices(const WsgFrame *frame, const uint8_t *prom)
{
    const uint32_t next = audio_posted + 1u;
    audio_slots[next & 1u] = frame != 0 ? *frame : wsg_silent_frame();
    audio_prom = prom;
    AUDIO_BARRIER();
    audio_posted = next;
}

/** In the interrupt: takes the latest voices posted, which start with their first tick. */
static void audio_take_voices(void)
{
    const uint32_t posted = audio_posted;
    if (posted == audio_taken)
        return;
    audio_frame = audio_slots[posted & 1u];
    audio_taken = posted;
    wsg_start_frame(&audio_wsg);
}

/** Asks for `effect` to start at the next sound interrupt (dropped if the queue is full). */
static void audio_play(const SoundEffect *effect)
{
    const uint32_t w = audio_written;
    if (w - audio_read >= AUDIO_QUEUE)
        return;
    audio_queue[w % AUDIO_QUEUE] = effect;
    audio_written = w + 1u; /* after the slot is filled: the interrupt may read it from now on */
}

/** Whether a sound is playing (read from the main loop: a moment's delay does not matter). */
static int audio_is_playing(void)
{
    return audio_written != audio_read || mixer_is_playing(&audio_mixer);
}

/** In app_audio(): starts the effects asked for, then fills `n` stereo frames (Q15). */
static void audio_render(int32_t *out, uint32_t n)
{
    while (audio_read != audio_written) {
        mixer_play(&audio_mixer, audio_queue[audio_read % AUDIO_QUEUE]);
        audio_read++;
    }
    audio_take_voices();
    const uint8_t *prom = audio_prom;
    for (uint32_t done = 0; done < n;) {
        const uint32_t count = n - done < AUDIO_BLOCK ? n - done : AUDIO_BLOCK;
        for (uint32_t i = 0; i < count; i++)
            audio_mix[i] = 0;
        mixer_render(&audio_mixer, audio_mix, count);
        if (prom != 0)
            wsg_render(&audio_wsg, &audio_frame, prom, audio_mix, count);
        for (uint32_t i = 0; i < count; i++) {
            out[2u * (done + i)] += audio_mix[i]; /* the same on both sides: mono, as the cabinet */
            out[2u * (done + i) + 1u] += audio_mix[i];
        }
        done += count;
    }
}

#endif
