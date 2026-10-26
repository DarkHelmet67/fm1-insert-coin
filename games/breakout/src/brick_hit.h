/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The ball against the bricks (brick-hit.ts).
 */
#ifndef BREAKOUT_BRICK_HIT_H
#define BREAKOUT_BRICK_HIT_H

#include <stdint.h>

#include "ball.h"
#include "bricks.h"
#include "cartridge/private.h"

/** The ball, the wall and the points after one frame of brick collisions. */
typedef struct {
    Ball ball;
    Wall wall;
    int32_t points;
} BrickHit;

/**
 * Breaks the brick the ball touches, if any: the brick goes and the ball turns back vertically.
 * After a hit the ball passes through every other brick until it touches the paddle or the top
 * wall: one brick per trip, as in the circuit.
 */
GAME_PRIVATE BrickHit brick_hit(const Ball *ball, const Wall *wall);

#endif
