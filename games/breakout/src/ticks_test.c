/* SPDX-License-Identifier: GPL-3.0-only */
#include "test/check.h"
#include "ticks.h"

static void a_red_brick_ticks_seven_times_five_frames_apart(void)
{
    Ticks ticks = ticks_none();
    uint32_t first_frames[7];
    uint32_t count = 0;
    for (uint32_t frame = 0; frame < 60; frame++) {
        const uint32_t before = ticks.played;
        ticks = ticks_step(&ticks, frame == 0 ? 7 : 0);
        if (ticks.played > before && count < 7)
            first_frames[count++] = frame;
    }
    CHECK_EQ(ticks.played, 7);
    CHECK_EQ(ticks.owed, 0);
    CHECK_EQ(first_frames[0], 0);
    CHECK_EQ(first_frames[1], 5);
    CHECK_EQ(first_frames[6], 30);
}

static void nothing_owed_nothing_played(void)
{
    const Ticks none = ticks_none();
    CHECK_EQ(ticks_step(&none, 0).played, 0);
}

int main(void)
{
    RUN(a_red_brick_ticks_seven_times_five_frames_apart);
    RUN(nothing_owed_nothing_played);
    return check_report();
}
