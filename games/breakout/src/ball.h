/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The ball in play (ball.ts). Like the circuit, it keeps directions and a few flags, not a
 * velocity: the speed comes from the flags at every frame (speed_for).
 */
#ifndef BREAKOUT_BALL_H
#define BREAKOUT_BALL_H

#include <stdint.h>

#include "cartridge/private.h"
#include "paddle.h"

/** The ball. Its position is in fifths (FIFTHS) of a scan line and of a step. */
typedef struct {
    int32_t x_fifths;      /**< left edge */
    int32_t y_fifths;      /**< top edge */
    int8_t dir_x;          /**< -1 left, 1 right */
    int8_t dir_y;          /**< -1 up (towards the bricks), 1 down (towards the paddle) */
    uint8_t can_hit_brick; /**< 0 after a brick, until the paddle or the top wall */
    uint8_t fast;          /**< an orange or red brick was hit: top speed until the next serve */
    uint8_t outer;         /**< the ball last touched an outer segment of the paddle */
    uint32_t hits;         /**< hits counted since the serve */
} Ball;

/**
 * Whether the ball overlaps a rectangle given in scan lines and steps.
 * @param ball the ball
 * @param x, y, width, height the rectangle
 */
GAME_PRIVATE int ball_overlaps(const Ball *ball, int32_t x, int32_t y, int32_t width,
                               int32_t height);

/** Moves the ball by one frame at its current speed. */
GAME_PRIVATE Ball ball_move(const Ball *ball);

/** Bounces the ball off the side walls and the top wall. */
GAME_PRIVATE Ball ball_bounce_off_walls(const Ball *ball);

/** Whether the ball touches the top wall in this frame: the paddle shrinks to half. */
GAME_PRIVATE int ball_touches_top_wall(const Ball *ball);

/** Which quarter of the paddle the ball's centre is over: 0 and 3 are the outer segments. */
GAME_PRIVATE int32_t ball_paddle_segment(const Ball *ball, const Paddle *paddle);

/**
 * The ball after the paddle, if it hits it while coming down: back up, to the side of the half
 * it touched, flatter off the outer segments. Every hit counts towards the speed-ups.
 */
GAME_PRIVATE Ball ball_bounce_off_paddle(const Ball *ball, const Paddle *paddle);

/** Whether the ball has left the bottom of the screen. */
GAME_PRIVATE int ball_is_lost(const Ball *ball);

/** The ball's position in whole pixels, as it is drawn: the nearest one. */
GAME_PRIVATE int32_t ball_pixel(int32_t fifths);

#endif
