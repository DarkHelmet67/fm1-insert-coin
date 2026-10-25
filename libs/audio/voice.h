/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One sound being played: an oscillator or a noise source, two low-pass filter stages and a
 * volume envelope, all in integers. Rendering changes the voice, like drawing changes a
 * framebuffer: it runs in the sound interrupt, 44,100 times a second.
 */
#ifndef VOICE_H
#define VOICE_H

#include <stdint.h>

#include "sound.h"

/** The sample rate of the FM-1 (platform.c: FS). */
#define AUDIO_RATE 44100

/** Pitch and volume are recomputed every this many samples (0.7 ms), not at every sample. */
#define CONTROL_SAMPLES 32

/** A sound being played. */
typedef struct {
    const Sound *sound;        /**< NULL when the voice is free */
    int32_t position;          /**< samples since the start; negative during the delay */
    int32_t length;            /**< samples from the start to the end */
    int32_t hold;              /**< samples at full volume */
    int32_t sweep_octaves_q16; /**< log2(to / from), for an exponential sweep */
    int32_t fade_octaves_q16;  /**< octaves the volume falls in an exponential fade */
    int32_t filter_q15;        /**< low-pass coefficient, 0 = no filter */
    uint32_t phase;            /**< oscillator phase, a full turn is 2^32 */
    uint32_t increment;        /**< phase step per sample, for the current pitch */
    int32_t gain_q15;          /**< volume for the current block */
    uint32_t noise;            /**< noise generator state */
    int32_t low1, low2;        /**< the two filter stages */
} Voice;

/** A voice that plays nothing. */
Voice voice_silent(void);

/** A voice about to play `sound`. */
Voice voice_start(const Sound *sound);

/** Whether the voice has finished (or never started) and can play something else. */
int voice_is_free(const Voice *voice);

/**
 * Adds `n` samples of the voice to `mix` (Q15, one channel) and moves the voice on.
 * The voice frees itself at the end of its sound.
 */
void voice_render(Voice *voice, int32_t *mix, uint32_t n);

#endif
