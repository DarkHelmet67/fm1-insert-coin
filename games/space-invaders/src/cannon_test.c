/* SPDX-License-Identifier: GPL-3.0-only */
#include "cannon.h"
#include "test/check.h"

static void it_moves_one_pixel_per_frame(void)
{
    CHECK_EQ(cannon_move(100, 1), 101);
    CHECK_EQ(cannon_move(100, -1), 99);
    CHECK_EQ(cannon_move(100, 0), 100);
}

static void it_stops_at_the_screen_edges(void)
{
    CHECK_EQ(cannon_move(CANNON_MIN_X, -1), CANNON_MIN_X);
    CHECK_EQ(cannon_move(CANNON_MAX_X, 1), CANNON_MAX_X);
}

static void the_shot_starts_at_the_tip_of_the_cannon(void)
{
    const Shot shot = shot_fire(100);
    CHECK(shot.active);
    CHECK_EQ(shot.x, 106);
    CHECK_EQ(shot.y, CANNON_Y - 4);
}

static void the_shot_moves_up_less_than_an_invader_height(void)
{
    const Shot shot = shot_fire(100);
    const Shot moved = shot_move(&shot);
    CHECK_EQ(moved.y, shot.y - SHOT_SPEED);
    CHECK(SHOT_SPEED < 8);
}

static void the_shot_disappears_above_the_playfield(void)
{
    const Shot high = {1, 50, SHOT_TOP_LIMIT + 3};
    CHECK(!shot_move(&high).active);
    const Shot just = {1, 50, SHOT_TOP_LIMIT + 4};
    CHECK(shot_move(&just).active);
}

int main(void)
{
    RUN(it_moves_one_pixel_per_frame);
    RUN(it_stops_at_the_screen_edges);
    RUN(the_shot_starts_at_the_tip_of_the_cannon);
    RUN(the_shot_moves_up_less_than_an_invader_height);
    RUN(the_shot_disappears_above_the_playfield);
    return check_report();
}
