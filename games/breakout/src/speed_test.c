/* SPDX-License-Identifier: GPL-3.0-only */
#include "speed.h"
#include "test/check.h"

static void the_speed_climbs_with_the_hits(void)
{
    CHECK_EQ(speed_for(0, 0, 0).vertical, 3);
    CHECK_EQ(speed_for(0, 0, 0).sideways, 3);
    CHECK_EQ(speed_for(0, 0, 1).sideways, 6);
    CHECK_EQ(speed_for(4, 0, 0).vertical, 4);
    CHECK_EQ(speed_for(8, 0, 0).sideways, 8);
    CHECK_EQ(speed_for(100, 0, 0).vertical, 4);
}

static void a_high_brick_gives_the_top_speed(void)
{
    CHECK_EQ(speed_for(0, 1, 0).vertical, 5);
    CHECK_EQ(speed_for(0, 1, 0).sideways, 8);
}

int main(void)
{
    RUN(the_speed_climbs_with_the_hits);
    RUN(a_high_brick_gives_the_top_speed);
    return check_report();
}
