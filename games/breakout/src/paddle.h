/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The paddle: its left edge and its width, always on the paddle row (paddle.ts).
 */
#ifndef BREAKOUT_PADDLE_H
#define BREAKOUT_PADDLE_H

#include <stdint.h>

#include "cartridge/private.h"
#include "controls.h"
#include "playfield.h"

/** Width of the full paddle, and of the paddle after the ball touched the top wall. */
#define PADDLE_WIDTH 16
#define HALF_PADDLE_WIDTH (PADDLE_WIDTH / 2)

/** A paddle as wide as the whole space between the walls: the attract mode's. */
#define FULL_ROW_WIDTH (RIGHT_WALL_X - SIDE_WALL_WIDTH)

/** The paddle, in scan lines. */
typedef struct {
    int32_t x;
    int32_t width;
} Paddle;

/** The paddle's width: full, or half after a breakout until the next serve. */
GAME_PRIVATE int32_t paddle_width(int shrunk);

/** The paddle at the start: full width, in the middle. */
GAME_PRIVATE Paddle paddle_initial(void);

/** The attract mode's paddle: the whole row, so the ball bounces forever. */
GAME_PRIVATE Paddle paddle_attract(void);

/**
 * Moves the paddle for one frame: its centre goes where the pointer is (on the FM-1, where the
 * knobs turned it), otherwise the arrow keys push it; it stays between the walls.
 */
GAME_PRIVATE Paddle paddle_move(const Paddle *paddle, const Controls *controls);

#endif
