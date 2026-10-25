/* SPDX-License-Identifier: GPL-3.0-only */
#include "fighter.h"
#include "objects.h"
#include "rockets.h"
#include "test/check.h"

static void an_upright_fighter_fires_straight_up(void)
{
    GameObject fighter = no_object();
    fighter.code = 6;
    const Rocket rocket = rocket_for(&fighter);
    CHECK_EQ(rocket.code, 0x30);
    CHECK_EQ(rocket.path.dx, 0);
    CHECK_EQ(rocket.path.dy, -6);
}

static void a_fighter_turned_45_degrees_fires_along_the_diagonal(void)
{
    GameObject fighter = no_object();
    fighter.code = 3;
    fighter.flip_x = 1;
    const Rocket rocket = rocket_for(&fighter);
    CHECK_EQ(rocket.code, 0x31);
    CHECK_EQ(rocket.path.dx, 6);
    CHECK_EQ(rocket.path.dy, -6);
}

static void a_horizontal_fighter_fires_sideways(void)
{
    GameObject fighter = no_object();
    fighter.code = 0;
    const Rocket rocket = rocket_for(&fighter);
    CHECK_EQ(rocket.code, 0x33);
    CHECK_EQ(rocket.path.dx, -6);
    CHECK_EQ(rocket.path.dy, 0);
}

static void at_most_two_rockets_fly(void)
{
    Objects objects = empty_objects();
    RocketPaths paths = no_paths();
    place_fighter(&objects);
    CHECK(fire_rocket(&objects, &paths, 0));
    CHECK(fire_rocket(&objects, &paths, 0));
    CHECK(!fire_rocket(&objects, &paths, 0));
    CHECK_EQ(object_at(&objects, FIRST_ROCKET + 2)->x, object_at(&objects, FIGHTER)->x);
}

static void a_double_fighter_fires_a_double_width_rocket(void)
{
    Objects objects = empty_objects();
    RocketPaths paths = no_paths();
    place_fighter(&objects);
    fire_rocket(&objects, &paths, 1);
    CHECK(object_at(&objects, FIRST_ROCKET)->wide);
}

static void a_rocket_climbs_6_pixels_a_frame_and_is_freed_at_the_top(void)
{
    Objects objects = empty_objects();
    RocketPaths paths = no_paths();
    place_fighter(&objects);
    fire_rocket(&objects, &paths, 0);
    const int32_t y = object_at(&objects, FIRST_ROCKET)->y;
    move_rockets(&objects, &paths);
    CHECK_EQ(object_at(&objects, FIRST_ROCKET)->y, y - 6);
    object_ref(&objects, FIRST_ROCKET)->y = 44;
    move_rockets(&objects, &paths);
    CHECK_EQ(object_at(&objects, FIRST_ROCKET)->x, 0);
}

int main(void)
{
    RUN(an_upright_fighter_fires_straight_up);
    RUN(a_fighter_turned_45_degrees_fires_along_the_diagonal);
    RUN(a_horizontal_fighter_fires_sideways);
    RUN(at_most_two_rockets_fly);
    RUN(a_double_fighter_fires_a_double_width_rocket);
    RUN(a_rocket_climbs_6_pixels_a_frame_and_is_freed_at_the_top);
    return check_report();
}
