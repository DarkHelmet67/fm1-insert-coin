/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound effects on the FM-1: the main loop asks for effects with audio_play(), the sound
 * interrupt mixes them with audio_render(), called from app_audio(). Include from main.c, after
 * platform.c.
 *
 * The two sides meet in a small queue: only the main loop writes `audio_written`, only the
 * interrupt writes `audio_read`, so neither needs to switch the other off.
 */
#ifndef AUDIO_FM1_H
#define AUDIO_FM1_H

#include "audio/mixer.h"

/** Effects asked for and not yet started: more than a frame ever asks for. */
#define AUDIO_QUEUE 16u

/** The frames the platform asks for at once (platform.c: HALF_FRAMES). */
#define AUDIO_BLOCK 256u

static const SoundEffect *volatile audio_queue[AUDIO_QUEUE];
static volatile uint32_t audio_written, audio_read;
static Mixer audio_mixer;
static int32_t audio_mix[AUDIO_BLOCK];

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
    for (uint32_t done = 0; done < n;) {
        const uint32_t count = n - done < AUDIO_BLOCK ? n - done : AUDIO_BLOCK;
        for (uint32_t i = 0; i < count; i++)
            audio_mix[i] = 0;
        mixer_render(&audio_mixer, audio_mix, count);
        for (uint32_t i = 0; i < count; i++) {
            out[2u * (done + i)] += audio_mix[i]; /* the same on both sides: mono, as the cabinet */
            out[2u * (done + i) + 1u] += audio_mix[i];
        }
        done += count;
    }
}

#endif
