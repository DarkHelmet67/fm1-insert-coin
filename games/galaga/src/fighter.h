/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The player's fighter (fighter.ts): where it appears and how it moves (`c_1F92`).
 */
#ifndef GALAGA_FIGHTER_H
#define GALAGA_FIGHTER_H

#include <stdint.h>

#include "cartridge/private.h"
#include "objects.h"

/** Where the fighter appears (`c_player_respawn`): centered at the bottom of the screen. */
#define FIGHTER_START_X 0x7a
#define FIGHTER_START_Y 0x129

/** The second fighter of a double fighter flies 15 pixels to the right of the first. */
#define DUAL_SPACING 0x0f

/** Sprite palette of the player's fighter (white, red and blue). */
#define FIGHTER_COLOR 9

/** Sprite code of the upright fighter. */
#define FIGHTER_UPRIGHT 6

/** Places the fighter at its start position, upright. */
GAME_PRIVATE void place_fighter(Objects *objects);

/**
 * `c_1F92`: moves the fighter in `direction` (-1, 0, 1). A held direction gives steps of 1 and 2
 * pixels in turn, 1.5 pixels a frame on average (`step_toggle`, `b_92A0[3]`, alternates); the
 * second fighter of a double fighter follows.
 */
GAME_PRIVATE void move_fighter(Objects *objects, uint8_t *step_toggle, int32_t direction, int dual);

#endif
