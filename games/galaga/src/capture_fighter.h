/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The captured fighter (capture-fighter.ts, `f_20F2`, `f_19B2`): it spins up the beam to the
 * Boss, turns red, and the Boss takes it home, where it rests above it as an enemy. "FIGHTER
 * CAPTURED" is shown meanwhile, and the stage restarts with the next fighter.
 */
#ifndef GALAGA_CAPTURE_FIGHTER_H
#define GALAGA_CAPTURE_FIGHTER_H

#include "cartridge/private.h"
#include "playing.h"

/**
 * `c_2188_ship_spin`: one step of a fighter spinning clockwise. The 7 sprite codes 0-6 draw a
 * quarter turn; the two flip bits give the quarter, in Gray code. The screen is rotated, so the
 * hardware's first flip bit mirrors the sprite vertically. `keep_spinning` lets it go on past
 * the upright position. Returns 1, without changing it, when it is upright and told to stop.
 */
GAME_PRIVATE int spin(GameObject *object, int keep_spinning);

/** One pixel closer to `target`. */
GAME_PRIVATE int32_t toward(int32_t value, int32_t target);

/** `f_20F2`: one frame of the fighter caught in the beam. */
GAME_PRIVATE void pull_fighter(PlayingState *state);

/** `f_19B2`: one frame of the Boss carrying the captured fighter home. */
GAME_PRIVATE void carry_fighter(PlayingState *state);

#endif
