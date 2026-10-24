/* SPDX-License-Identifier: GPL-3.0-only */
#include "test/check.h"
#include "ufo.h"

static void it_gives_300_points_on_the_23rd_shot_and_every_15_shots_after(void)
{
    CHECK_EQ(ufo_score(8), 300);
    CHECK_EQ(ufo_score(23), 300);
    CHECK_EQ(ufo_score(38), 300);
}

static void it_gives_50_to_150_points_on_the_other_shots(void)
{
    for (uint32_t shots = 0; shots < 15; shots++)
        if (shots != 8)
            CHECK(ufo_score(shots) >= 50 && ufo_score(shots) <= 150);
}

static void it_counts_down_between_visits(void)
{
    const Ufo start = ufo_initial();
    const Ufo next = ufo_step(&start, 55, 0);
    CHECK_EQ(next.countdown, UFO_INTERVAL - 1);
    CHECK(!next.flying);
}

static void it_launches_the_ship_when_the_countdown_expires(void)
{
    Ufo ufo = ufo_initial();
    ufo.countdown = 0;
    const Ufo next = ufo_step(&ufo, 55, 0);
    CHECK(next.flying);
    CHECK_EQ(next.countdown, UFO_INTERVAL);
}

static void it_skips_the_visit_when_fewer_than_8_invaders_are_left(void)
{
    Ufo ufo = ufo_initial();
    ufo.countdown = 0;
    const Ufo next = ufo_step(&ufo, 7, 0);
    CHECK(!next.flying);
    CHECK_EQ(next.countdown, UFO_INTERVAL);
}

static void it_enters_from_the_side_chosen_by_the_parity_of_the_shots(void)
{
    Ufo ufo = ufo_initial();
    ufo.countdown = 0;
    const Ufo even = ufo_step(&ufo, 55, 4);
    const Ufo odd = ufo_step(&ufo, 55, 5);
    CHECK_EQ(even.x, UFO_LEFT_X);
    CHECK_EQ(even.direction, 1);
    CHECK_EQ(odd.x, UFO_RIGHT_X);
    CHECK_EQ(odd.direction, -1);
}

static void it_flies_one_pixel_per_frame_and_leaves_at_the_other_side(void)
{
    Ufo ufo = ufo_initial();
    ufo.countdown = 0;
    ufo = ufo_step(&ufo, 55, 0);
    ufo = ufo_step(&ufo, 55, 0);
    CHECK_EQ(ufo.x, UFO_LEFT_X + 1);
    CHECK_EQ(ufo.age, 1u);
    for (int i = 0; i < UFO_RIGHT_X - UFO_LEFT_X; i++)
        ufo = ufo_step(&ufo, 55, 0);
    CHECK(!ufo.flying);
}

int main(void)
{
    RUN(it_gives_300_points_on_the_23rd_shot_and_every_15_shots_after);
    RUN(it_gives_50_to_150_points_on_the_other_shots);
    RUN(it_counts_down_between_visits);
    RUN(it_launches_the_ship_when_the_countdown_expires);
    RUN(it_skips_the_visit_when_fewer_than_8_invaders_are_left);
    RUN(it_enters_from_the_side_chosen_by_the_parity_of_the_shots);
    RUN(it_flies_one_pixel_per_frame_and_leaves_at_the_other_side);
    return check_report();
}
