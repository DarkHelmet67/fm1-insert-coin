/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What the game asks of the sound processor in one frame (sound-commands.ts), as the main
 * processor writes its trigger registers (`b_9AA0`): the sounds started in this frame, the sounds
 * that last as long as something happens, read from the state, and the direction of the breathing
 * formation.
 */
#ifndef GALAGA_SOUND_COMMANDS_H
#define GALAGA_SOUND_COMMANDS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "playing.h"

/** At most this many writes in a frame: the 15 started sounds, 4 held ones and the pulse stop. */
#define MAX_SOUND_WRITES 20

/** A write to a trigger register. */
typedef struct {
    uint8_t effect; /**< EFFECT_* */
    uint8_t value;
} SoundWrite;

/** The sound commands of one frame. */
typedef struct {
    SoundWrite writes[MAX_SOUND_WRITES];
    int32_t count;
    uint8_t explosion; /**< the fighter explodes: a noise from another chip, the 54XX */
    int32_t sign; /**< `formatn_mv_signage`: 1 while the formation opens, $FF while it closes */
} SoundCommands;

/** Nothing to play: the attract screen. */
GAME_PRIVATE void no_sound_commands(SoundCommands *commands);

/**
 * The formation's direction: the sign of the breathing counter before its last step. The counter
 * only changes sign at $1F, which steps to $A0, and at $81, which steps to $00.
 */
GAME_PRIVATE int32_t formation_sign(uint8_t direction);

/** The sound commands of a frame of play. */
GAME_PRIVATE void sound_commands(const PlayingState *state, SoundCommands *commands);

#endif
