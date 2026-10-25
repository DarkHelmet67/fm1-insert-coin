/* SPDX-License-Identifier: GPL-3.0-only */
#include "paddle.h"

#include "config.h"

int32_t paddle_width(int shrunk)
{
    return shrunk ? HALF_PADDLE_WIDTH : PADDLE_WIDTH;
}

Paddle paddle_initial(void)
{
    /* Math.round((4 + 224 - 16) / 2): an exact half */
    const Paddle paddle = {(SIDE_WALL_WIDTH + RIGHT_WALL_X - PADDLE_WIDTH) / 2, PADDLE_WIDTH};
    return paddle;
}

Paddle paddle_attract(void)
{
    const Paddle paddle = {SIDE_WALL_WIDTH, FULL_ROW_WIDTH};
    return paddle;
}

/** Keeps the paddle between the side walls. */
static int32_t keep_inside(int32_t x, int32_t width)
{
    const int32_t right = RIGHT_WALL_X - width;
    return x < SIDE_WALL_WIDTH ? SIDE_WALL_WIDTH : x > right ? right : x;
}

Paddle paddle_move(const Paddle *paddle, const Controls *controls)
{
    /* the widths are even, so the pointer's centre minus half the width stays whole */
    const int32_t x = controls->has_pointer ? controls->pointer_x - paddle->width / 2
                                            : paddle->x + controls->direction * PADDLE_KEY_SPEED;
    Paddle next = *paddle;
    next.x = keep_inside(x, paddle->width);
    return next;
}
