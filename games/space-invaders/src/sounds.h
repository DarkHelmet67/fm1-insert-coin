/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound effects and when they play (insert-coin, sound-bank.ts and sounds.ts). Sounds are
 * found by comparing the state before and after a frame, so the game logic knows nothing about
 * audio, as in the web version.
 */
#ifndef SOUNDS_H
#define SOUNDS_H

#include <stdint.h>

#include "audio/sound.h"
#include "cartridge/private.h"
#include "game.h"

/** The sound effects of the game. */
typedef enum {
    SOUND_COIN,
    SOUND_SHOT,
    SOUND_ALIEN_HIT,
    SOUND_UFO,
    SOUND_UFO_HIT,
    SOUND_CANNON_HIT,
    SOUND_EXTRA_LIFE,
    SOUND_NEW_RECORD,
    SOUND_MARCH_0,
    SOUND_MARCH_1,
    SOUND_MARCH_2,
    SOUND_MARCH_3,
    SOUND_COUNT
} SoundName;

/** The bit of a sound in the mask returned by sounds_for. */
#define SOUND_BIT(name) (1u << (name))

/** The effect of each sound, synthesized from a few numbers measured on the original. */
GAME_EXTERN const SoundEffect SOUND_EFFECTS[SOUND_COUNT];

/** The sounds to play for the frame that turned `previous` into `next`, as SOUND_BIT()s. */
GAME_PRIVATE uint32_t sounds_for(const GameState *previous, const GameState *next);

#endif
