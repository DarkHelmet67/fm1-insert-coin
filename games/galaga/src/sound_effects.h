/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The three ways the sound program plays an effect (sound-effects.ts, `c_03F4`/`c_044A`,
 * `c_0375`, `c_04A2` in `gg1-7.s`). They differ in what the trigger register means:
 * - a *restarting* effect (hits, shot, dive) starts again from the top at every trigger, which
 *   the program clears at once;
 * - a *held* effect (tractor beam, rescue) plays, in a loop, as long as its register is set;
 * - a *tune* plays to its end once triggered, then clears its register.
 */
#ifndef GALAGA_SOUND_EFFECTS_H
#define GALAGA_SOUND_EFFECTS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "sound_state.h"

/** `c_03F4`, `c_044A`: one tick of a restarting effect. */
GAME_PRIVATE void restarting(Work *work, int32_t effect);

/** `c_0375`: one tick of a held effect; when its register is cleared it stops. */
GAME_PRIVATE void held(Work *work, int32_t effect);

/** `c_04A2`: one tick of a tune, played to its end once its register is set. */
GAME_PRIVATE void tune(Work *work, int32_t effect);

/** Sets voice `voice` to `volume` after the effects, as a few of them do. */
GAME_PRIVATE void set_volume(Work *work, int32_t voice, int32_t volume);

#endif
