/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound effects and when they play (sound-bank.ts and sounds.ts): square-wave beeps, the
 * only sound the circuit could make, found by comparing the state before and after a frame.
 */
#ifndef BREAKOUT_SOUNDS_H
#define BREAKOUT_SOUNDS_H

#include <stdint.h>

#include "audio/sound.h"
#include "cartridge/private.h"
#include "game.h"

/** The sound effects of the game. */
typedef enum { SOUND_PADDLE, SOUND_WALL, SOUND_BRICK, SOUND_NEW_RECORD, SOUND_COUNT } SoundName;

/** The bit of a sound in the mask returned by sounds_for. */
#define SOUND_BIT(name) (1u << (name))

/** The effect of each sound. */
GAME_EXTERN const SoundEffect SOUND_EFFECTS[SOUND_COUNT];

/**
 * The sounds to play for the frame that turned `previous` into `next`, as SOUND_BIT()s. The
 * attract mode is silent, as on the cabinet.
 */
GAME_PRIVATE uint32_t sounds_for(const GameState *previous, const GameState *next);

#endif
