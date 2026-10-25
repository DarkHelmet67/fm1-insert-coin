/* SPDX-License-Identifier: GPL-3.0-only */
/** @file Several voices played together: the sound effects of a game, mixed into one signal. */
#ifndef MIXER_H
#define MIXER_H

#include "voice.h"

/** More voices than a game ever needs at once (an effect can have several layers). */
#define MIXER_VOICES 16

/** The voices being played. */
typedef struct {
    Voice voices[MIXER_VOICES];
    uint32_t
        next; /**< where the search for a free voice starts: a full mixer replaces the oldest */
} Mixer;

/** A mixer with every voice free. */
Mixer mixer_silent(void);

/** Starts every layer of `effect` on a free voice (the oldest voice when none is free). */
void mixer_play(Mixer *mixer, const SoundEffect *effect);

/** Stops every voice. */
void mixer_stop(Mixer *mixer);

/** Adds `n` samples of every voice to `mix` (Q15, one channel). */
void mixer_render(Mixer *mixer, int32_t *mix, uint32_t n);

/** Whether any voice is playing. */
int mixer_is_playing(const Mixer *mixer);

#endif
