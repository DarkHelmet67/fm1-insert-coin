/* SPDX-License-Identifier: GPL-3.0-only */
#include "paddle.h"
#include "test/check.h"

static Controls none(void)
{
    const Controls controls = {0, 0, 0, 0, 0};
    return controls;
}

static void the_paddle_starts_in_the_middle(void)
{
    CHECK_EQ(paddle_initial().x, 106);
    CHECK_EQ(paddle_initial().width, PADDLE_WIDTH);
    CHECK_EQ(paddle_attract().width, FULL_ROW_WIDTH);
    CHECK_EQ(paddle_width(1), HALF_PADDLE_WIDTH);
}

static void the_keys_move_it_and_the_walls_stop_it(void)
{
    Controls controls = none();
    controls.direction = 1;
    const Paddle start = paddle_initial();
    CHECK_EQ(paddle_move(&start, &controls).x, 109);
    const Paddle right = {RIGHT_WALL_X - PADDLE_WIDTH, PADDLE_WIDTH};
    CHECK_EQ(paddle_move(&right, &controls).x, RIGHT_WALL_X - PADDLE_WIDTH);
}

static void the_pointer_places_its_centre(void)
{
    Controls controls = none();
    controls.has_pointer = 1;
    controls.pointer_x = 50;
    const Paddle start = paddle_initial();
    CHECK_EQ(paddle_move(&start, &controls).x, 42);
    controls.pointer_x = -100;
    CHECK_EQ(paddle_move(&start, &controls).x, SIDE_WALL_WIDTH);
}

int main(void)
{
    RUN(the_paddle_starts_in_the_middle);
    RUN(the_keys_move_it_and_the_walls_stop_it);
    RUN(the_pointer_places_its_centre);
    return check_report();
}
