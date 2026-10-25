/* SPDX-License-Identifier: GPL-3.0-only */
#include "brick_hit.h"
#include "test/check.h"

/** A ball going up into the bottom row's first brick. */
static Ball rising_ball(void)
{
    Ball ball = {2 * FIFTHS, (BRICKS_TOP + 7 * BRICK_HEIGHT + 2) * FIFTHS, 1, -1, 1, 0, 0, 3};
    return ball;
}

static void a_brick_breaks_and_turns_the_ball(void)
{
    const Wall wall = wall_full();
    const Ball ball = rising_ball();
    const BrickHit hit = brick_hit(&ball, &wall);
    CHECK_EQ(hit.points, 1);
    CHECK_EQ(hit.wall.standing[7 * BRICK_COLUMNS], 0);
    CHECK_EQ(hit.ball.dir_y, 1);
    CHECK_EQ(hit.ball.can_hit_brick, 0);
    CHECK_EQ(hit.ball.fast, 0);
}

static void after_a_hit_the_ball_passes_through(void)
{
    const Wall wall = wall_full();
    Ball ball = rising_ball();
    ball.can_hit_brick = 0;
    CHECK_EQ(brick_hit(&ball, &wall).points, 0);
}

static void a_red_brick_speeds_the_ball_up(void)
{
    Wall wall = wall_full();
    Ball ball = rising_ball();
    ball.y_fifths = (BRICKS_TOP + 2) * FIFTHS;
    ball.dir_y = 1; /* came down from above the wall: sent back up, a hit */
    const BrickHit hit = brick_hit(&ball, &wall);
    CHECK_EQ(hit.points, 7);
    CHECK_EQ(hit.ball.fast, 1);
    CHECK_EQ(hit.ball.hits, 4);
}

int main(void)
{
    RUN(a_brick_breaks_and_turns_the_ball);
    RUN(after_a_hit_the_ball_passes_through);
    RUN(a_red_brick_speeds_the_ball_up);
    return check_report();
}
