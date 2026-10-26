/* SPDX-License-Identifier: GPL-3.0-only */
#include "attract.h"

Round attract_step(const Round *round, uint32_t clock)
{
    const Paddle paddle = paddle_attract();
    Round next = round_step(round, &paddle, 1, clock);
    next.wall = round->wall;
    next.score = round->score;
    next.ball = round->ball;
    next.refilled = round->refilled;
    next.shrunk = 0;
    return next;
}
