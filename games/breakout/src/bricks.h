/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The wall of bricks (bricks.ts): one flag per brick, row by row from the top. The original
 * kept exactly this in a 256 x 1 RAM chip, one bit per brick.
 */
#ifndef BREAKOUT_BRICKS_H
#define BREAKOUT_BRICKS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "playfield.h"

/** How many bricks a wall has. */
#define BRICK_COUNT (BRICK_ROWS * BRICK_COLUMNS)

/** The bricks standing: 1 standing, 0 broken; index row * BRICK_COLUMNS + column. */
typedef struct {
    uint8_t standing[BRICK_COUNT];
} Wall;

/** A rectangle in scan lines and steps. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} BrickRect;

/** Points of a whole wall: 14 bricks x 2 rows x (7 + 5 + 3 + 1) = 448. */
#define WALL_POINTS 448

/** A complete wall: 8 rows of 14 bricks. */
GAME_PRIVATE Wall wall_full(void);

/** The row of brick `index`, 0 at the top. */
GAME_PRIVATE int32_t brick_row(uint32_t index);

/** Where brick `index` sits: the ball bounces on this rectangle. */
GAME_PRIVATE BrickRect brick_rect(uint32_t index);

/** The visible part of brick `index`: its rectangle minus the gap below each row. */
GAME_PRIVATE BrickRect brick_shape(uint32_t index);

/** Points for a brick in `row`: red 7, orange 5, green 3, yellow 1. */
GAME_PRIVATE int32_t brick_points(int32_t row);

/** Whether a brick in `row` is orange or red: it sends the ball to its top speed. */
GAME_PRIVATE int brick_is_high_row(int32_t row);

#endif
