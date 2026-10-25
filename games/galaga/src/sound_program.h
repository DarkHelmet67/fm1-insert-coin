/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `c_nmi_proc` (sound-program.ts): one tick of the sound processor, 120 a second. The effects are
 * checked in a fixed order and each one writes the voices it uses: an effect later in the list
 * covers an earlier one on the same voice, so the order is the priority.
 */
#ifndef GALAGA_SOUND_PROGRAM_H
#define GALAGA_SOUND_PROGRAM_H

#include <stdint.h>

#include "audio/wsg.h"
#include "cartridge/private.h"
#include "sound_state.h"

/**
 * One tick of the sound program on the processor's memory `sound`; `sign` is the formation's
 * direction (`formatn_mv_signage`: 1 opening, $FF closing). Writes what the three voices play.
 */
GAME_PRIVATE void tick_sound(SoundState *sound, int32_t sign, WsgVoice voices[WSG_VOICES]);

#endif
