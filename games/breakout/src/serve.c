/* SPDX-License-Identifier: GPL-3.0-only */
#include "serve.h"

#include "config.h"

/** How far the ball can be from the left wall when it appears. */
#define SERVE_RANGE (RIGHT_WALL_X - SIDE_WALL_WIDTH - BALL_WIDTH)

uint32_t serve_delay(uint32_t clock)
{
    return (SERVE_CYCLE_FRAMES - clock % SERVE_CYCLE_FRAMES) % SERVE_CYCLE_FRAMES;
}

Ball serve_ball(uint32_t pressed_at)
{
    Ball ball;
    ball.x_fifths = (SIDE_WALL_WIDTH + (int32_t)(pressed_at % SERVE_RANGE)) * FIFTHS;
    ball.y_fifths = SERVE_APPEAR_Y * FIFTHS;
    ball.dir_x = pressed_at % 2u == 0u ? 1 : -1;
    ball.dir_y = 1;
    ball.hits = 0;
    ball.fast = 0;
    ball.outer = pressed_at % 4u >= 2u;
    ball.can_hit_brick = 1;
    return ball;
}
