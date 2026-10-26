/* SPDX-License-Identifier: GPL-3.0-only */
#include "brick_hit.h"

/** The first standing brick the ball overlaps, row by row; BRICK_COUNT when none. */
static uint32_t touched_brick(const Ball *ball, const Wall *wall)
{
    for (uint32_t i = 0; i < BRICK_COUNT; i++) {
        const BrickRect rect = brick_rect(i);
        if (wall->standing[i] && ball_overlaps(ball, rect.x, rect.y, rect.width, rect.height))
            return i;
    }
    return BRICK_COUNT;
}

BrickHit brick_hit(const Ball *ball, const Wall *wall)
{
    BrickHit hit = {*ball, *wall, 0};
    const uint32_t index = ball->can_hit_brick ? touched_brick(ball, wall) : BRICK_COUNT;
    if (index == BRICK_COUNT)
        return hit;
    const int32_t row = brick_row(index);
    hit.ball.dir_y = ball->dir_y == 1 ? -1 : 1;
    hit.ball.can_hit_brick = 0;
    /* a ball sent back up by a brick counts as a hit too: the circuit counts every turn upwards */
    if (hit.ball.dir_y == -1)
        hit.ball.hits = ball->hits + 1u;
    hit.ball.fast = ball->fast || brick_is_high_row(row);
    hit.wall.standing[index] = 0;
    hit.points = brick_points(row);
    return hit;
}
