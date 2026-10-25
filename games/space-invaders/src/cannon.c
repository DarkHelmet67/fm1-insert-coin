/* SPDX-License-Identifier: GPL-3.0-only */
#include "cannon.h"

#include "sprites.h"

int32_t cannon_move(int32_t x, int32_t direction)
{
    const int32_t moved = x + direction;
    if (moved < CANNON_MIN_X)
        return CANNON_MIN_X;
    return moved > CANNON_MAX_X ? CANNON_MAX_X : moved;
}

int32_t cannon_center(int32_t x)
{
    return x + CANNON_WIDTH / 2;
}

Shot shot_fire(int32_t cannon_x)
{
    Shot shot = {1, (int16_t)cannon_center(cannon_x), (int16_t)(CANNON_Y - SHOT_SPRITE.height)};
    return shot;
}

Shot shot_move(const Shot *shot)
{
    Shot moved = *shot;
    moved.y = (int16_t)(shot->y - SHOT_SPEED);
    return moved.y < SHOT_TOP_LIMIT ? NO_SHOT : moved;
}
