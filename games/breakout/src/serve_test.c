/* SPDX-License-Identifier: GPL-3.0-only */
#include "serve.h"
#include "test/check.h"

static void the_ball_appears_when_the_cycle_comes_round(void)
{
    CHECK_EQ(serve_delay(0), 0);
    CHECK_EQ(serve_delay(1), 255);
    CHECK_EQ(serve_delay(255), 1);
    CHECK_EQ(serve_delay(256 + 6), 250);
}

static void the_press_frame_decides_where_and_how(void)
{
    const Ball even = serve_ball(10);
    CHECK_EQ(even.x_fifths, 14 * FIFTHS);
    CHECK_EQ(even.y_fifths, 120 * FIFTHS);
    CHECK_EQ(even.dir_x, 1);
    CHECK_EQ(even.dir_y, 1);
    CHECK_EQ(even.outer, 1);
    const Ball odd = serve_ball(217);
    CHECK_EQ(odd.dir_x, -1);
    CHECK_EQ(odd.x_fifths, 5 * FIFTHS); /* 217 wraps round the 216 scan lines */
}

int main(void)
{
    RUN(the_ball_appears_when_the_cycle_comes_round);
    RUN(the_press_frame_decides_where_and_how);
    return check_report();
}
