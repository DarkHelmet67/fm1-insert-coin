/* SPDX-License-Identifier: GPL-3.0-only */
#include "bricks.h"
#include "test/check.h"

static void a_wall_is_worth_448_points(void)
{
    int32_t points = 0;
    for (uint32_t i = 0; i < BRICK_COUNT; i++)
        points += brick_points(brick_row(i));
    CHECK_EQ(points, WALL_POINTS);
    CHECK_EQ(wall_full().standing[BRICK_COUNT - 1], 1);
}

static void the_bricks_sit_in_rows_with_a_gap(void)
{
    const BrickRect rect = brick_rect(BRICK_COLUMNS + 2);
    CHECK_EQ(rect.x, 2 * BRICK_PITCH);
    CHECK_EQ(rect.y, BRICKS_TOP + BRICK_HEIGHT);
    CHECK_EQ(brick_shape(0).height, BRICK_HEIGHT - 1);
}

static void the_top_rows_are_fast(void)
{
    CHECK(brick_is_high_row(0));
    CHECK(brick_is_high_row(3));
    CHECK(!brick_is_high_row(4));
    CHECK_EQ(brick_points(8), 0);
}

int main(void)
{
    RUN(a_wall_is_worth_448_points);
    RUN(the_bricks_sit_in_rows_with_a_gap);
    RUN(the_top_rows_are_fast);
    return check_report();
}
