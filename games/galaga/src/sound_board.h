/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound board of the cabinet (sound-system.ts): in every frame the main processor writes the
 * trigger registers, then the sound program ticks twice and sets the three voices of the WSG chip
 * for each tick. The chip itself plays in the sound interrupt (libs/audio/wsg.h).
 */
#ifndef GALAGA_SOUND_BOARD_H
#define GALAGA_SOUND_BOARD_H

#include "audio/wsg.h"
#include "cartridge/private.h"
#include "sound_commands.h"
#include "sound_state.h"

/**
 * One frame of the sound board: applies the writes of `commands` to the memory `sound`, then
 * ticks the sound program twice, writing the voices of each tick into `frame`.
 */
GAME_PRIVATE void sound_frame(SoundState *sound, const SoundCommands *commands, WsgFrame *frame);

#endif
