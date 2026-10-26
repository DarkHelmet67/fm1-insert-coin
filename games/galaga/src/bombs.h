/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_1EA4` (bombs.ts): the bombs fall 2 and 3 pixels on alternate frames and drift sideways at
 * the rate computed when they were dropped: a fixed-point rate of 32 steps per pixel, with the
 * remainder carried from frame to frame.
 */
#ifndef GALAGA_BOMBS_H
#define GALAGA_BOMBS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "flight_world.h"
#include "objects.h"

/** Sprite code and palette of a bomb (`c_game_or_demo_init`). */
#define BOMB_CODE 0x30
#define BOMB_COLOR 0x0b

/** Sets up the sprites of the bomb objects, off screen. */
GAME_PRIVATE void set_up_bombs(Objects *objects);

/** One frame of the 8 bombs. */
GAME_PRIVATE void move_bombs(Objects *objects, BombDrift drift[BOMB_COUNT], int32_t frame);

#endif
