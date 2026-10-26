/* SPDX-License-Identifier: GPL-3.0-only */
#include "ball.h"
#include "test/check.h"

/** A slow ball at (x, y) pixels, going down and right. */
static Ball ball_at(int32_t x, int32_t y)
{
    Ball ball = {x * FIFTHS, y * FIFTHS, 1, 1, 1, 0, 0, 0};
    return ball;
}

static void the_ball_moves_in_fifths(void)
{
    const Ball start = ball_at(100, 100);
    const Ball moved = ball_move(&start);
    CHECK_EQ(moved.x_fifths, 503);
    CHECK_EQ(moved.y_fifths, 503);
}

static void the_walls_turn_it_back(void)
{
    Ball ball = ball_at(RIGHT_WALL_X - BALL_WIDTH, 100);
    ball.x_fifths += 1;
    CHECK_EQ(ball_bounce_off_walls(&ball).dir_x, -1);
    CHECK_EQ(ball_bounce_off_walls(&ball).x_fifths, (RIGHT_WALL_X - BALL_WIDTH) * FIFTHS);
    ball = ball_at(100, TOP_WALL_HEIGHT);
    ball.dir_y = -1;
    ball.can_hit_brick = 0;
    CHECK_EQ(ball_bounce_off_walls(&ball).dir_y, 1);
    CHECK_EQ(ball_bounce_off_walls(&ball).can_hit_brick, 1);
}

static void the_paddle_segment_sets_the_direction(void)
{
    const Paddle paddle = {100, 16};
    Ball ball = ball_at(98, PADDLE_Y - 1); /* centre on the paddle's left edge: segment 0 */
    CHECK_EQ(ball_paddle_segment(&ball, &paddle), 0);
    Ball bounced = ball_bounce_off_paddle(&ball, &paddle);
    CHECK_EQ(bounced.dir_x, -1);
    CHECK_EQ(bounced.dir_y, -1);
    CHECK_EQ(bounced.outer, 1);
    CHECK_EQ(bounced.hits, 1);
    ball = ball_at(107, PADDLE_Y - 1); /* centre at 9 of 16: segment 2 */
    bounced = ball_bounce_off_paddle(&ball, &paddle);
    CHECK_EQ(bounced.dir_x, 1);
    CHECK_EQ(bounced.outer, 0);
}

static void a_ball_going_up_passes_the_paddle(void)
{
    const Paddle paddle = {100, 16};
    Ball ball = ball_at(104, PADDLE_Y - 1);
    ball.dir_y = -1;
    CHECK_EQ(ball_bounce_off_paddle(&ball, &paddle).dir_y, -1);
}

static void it_is_lost_past_the_bottom_and_drawn_on_the_nearest_pixel(void)
{
    const Ball low = ball_at(100, SCREEN_HEIGHT);
    CHECK(ball_is_lost(&low));
    CHECK_EQ(ball_pixel(547), 109);
    CHECK_EQ(ball_pixel(548), 110);
}

int main(void)
{
    RUN(the_ball_moves_in_fifths);
    RUN(the_walls_turn_it_back);
    RUN(the_paddle_segment_sets_the_direction);
    RUN(a_ball_going_up_passes_the_paddle);
    RUN(it_is_lost_past_the_bottom_and_drawn_on_the_nearest_pixel);
    return check_report();
}
