/* SPDX-License-Identifier: GPL-3.0-only */
#include "rom.h"
#include "stars.h"
#include "test/check.h"

/** The stars of a frame: fewer than the 256 the chip knows. */
static Star before[STAR_COUNT];
static Star after[STAR_COUNT];

static void the_chip_can_draw_256_stars(void)
{
    CHECK_EQ(STAR_COUNT, 256u);
}

static void two_of_the_four_sets_show_changing_every_4_frames(void)
{
    int32_t sets[2];
    star_sets(0, sets);
    CHECK_EQ(sets[0], 0);
    CHECK_EQ(sets[1], 2);
    star_sets(4, sets);
    CHECK_EQ(sets[0], 0);
    star_sets(8, sets);
    CHECK_EQ(sets[0], 1);
    CHECK_EQ(sets[1], 2);
}

static void the_stars_stand_still_at_speed_7(void)
{
    const int32_t phase = advance_stars(0, STARS_STILL);
    const int32_t count = visible_stars(0, STARS_STILL, 0, before);
    CHECK_EQ(visible_stars(phase, STARS_STILL, 0, after), count);
    CHECK_EQ(after[0].y, before[0].y);
}

static void the_stars_move_down_a_pixel_a_frame_at_speed_6(void)
{
    const int32_t count = visible_stars(0, STARS_SLOW, 0, before);
    const int32_t moved = visible_stars(advance_stars(0, STARS_SLOW), STARS_SLOW, 0, after);
    int32_t star = 0;
    while (star < count && (before[star].y <= 20 || before[star].y >= 250))
        star++;
    int found = 0;
    for (int32_t i = 0; i < moved; i++)
        found |= after[i].x == before[star].x && after[i].y == before[star].y + 1;
    CHECK(star < count);
    CHECK(found);
}

static void fewer_than_128_stars_show_all_inside_the_screen(void)
{
    const int32_t count = visible_stars(1234, STARS_SLOW, 0, before);
    int inside = 1;
    for (int32_t i = 0; i < count; i++)
        inside &= before[i].x < 224 && before[i].y >= 16;
    CHECK(count > 60);
    CHECK(count < 128);
    CHECK(inside);
}

int main(void)
{
    RUN(the_chip_can_draw_256_stars);
    RUN(two_of_the_four_sets_show_changing_every_4_frames);
    RUN(the_stars_stand_still_at_speed_7);
    RUN(the_stars_move_down_a_pixel_a_frame_at_speed_6);
    RUN(fewer_than_128_stars_show_all_inside_the_screen);
    return check_report();
}
