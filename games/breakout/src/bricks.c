/* SPDX-License-Identifier: GPL-3.0-only */
#include "bricks.h"

#include "config.h"

Wall wall_full(void)
{
    Wall wall;
    for (uint32_t i = 0; i < BRICK_COUNT; i++)
        wall.standing[i] = 1;
    return wall;
}

int32_t brick_row(uint32_t index)
{
    return (int32_t)(index / BRICK_COLUMNS);
}

BrickRect brick_rect(uint32_t index)
{
    const BrickRect rect = {(int32_t)(index % BRICK_COLUMNS) * BRICK_PITCH,
                            BRICKS_TOP + brick_row(index) * BRICK_HEIGHT, BRICK_WIDTH,
                            BRICK_HEIGHT};
    return rect;
}

BrickRect brick_shape(uint32_t index)
{
    BrickRect rect = brick_rect(index);
    rect.height -= BRICK_ROW_GAP;
    return rect;
}

int32_t brick_points(int32_t row)
{
    static const int32_t ROW_PAIR_POINTS[4] = {7, 5, 3, 1};
    return row >= 0 && row < BRICK_ROWS ? ROW_PAIR_POINTS[row / 2] : 0;
}

int brick_is_high_row(int32_t row)
{
    return brick_points(row) >= 5;
}
