/* SPDX-License-Identifier: GPL-3.0-only */
#include "fighter.h"
#include "objects.h"
#include "test/check.h"

static Objects objects;

/** Moves the fighter `frames` times in `direction` from its start; returns its x. */
static int32_t hold(int32_t direction, int32_t frames, int dual)
{
    uint8_t toggle = 0;
    objects = empty_objects();
    place_fighter(&objects);
    for (int32_t i = 0; i < frames; i++)
        move_fighter(&objects, &toggle, direction, dual);
    return object_at(&objects, FIGHTER)->x;
}

static void it_steps_1_then_2_pixels_3_every_2_frames(void)
{
    CHECK_EQ(hold(1, 1, 0), FIGHTER_START_X + 1);
    CHECK_EQ(hold(1, 2, 0), FIGHTER_START_X + 3);
    CHECK_EQ(hold(-1, 4, 0), FIGHTER_START_X - 6);
}

static void it_stops_at_the_screen_sides(void)
{
    const int32_t left = hold(-1, 200, 0);
    const int32_t right = hold(1, 200, 0);
    CHECK(left >= 0x10 && left < 0x12);
    CHECK(right >= 0xe1 && right <= 0xe2);
}

static void the_second_fighter_of_a_double_fighter_flies_15_pixels_right(void)
{
    const int32_t x = hold(1, 200, 1);
    CHECK(x >= 0xd1);
    CHECK_EQ(object_at(&objects, FIGHTER_2)->x, x + 15);
}

static void releasing_the_stick_resets_the_step(void)
{
    uint8_t toggle = 1;
    objects = empty_objects();
    place_fighter(&objects);
    move_fighter(&objects, &toggle, 0, 0);
    CHECK_EQ(toggle, 0);
}

int main(void)
{
    RUN(it_steps_1_then_2_pixels_3_every_2_frames);
    RUN(it_stops_at_the_screen_sides);
    RUN(the_second_fighter_of_a_double_fighter_flies_15_pixels_right);
    RUN(releasing_the_stick_resets_the_step);
    return check_report();
}
