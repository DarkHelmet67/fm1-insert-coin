/* SPDX-License-Identifier: GPL-3.0-only */
#include "clock.h"
#include "test/check.h"

static void the_slow_clock_steps_twice_every_32_frames(void)
{
    CHECK_EQ(slow_clock(1), 1);
    CHECK_EQ(slow_clock(31), 1);
    CHECK_EQ(slow_clock(32), 2);
    CHECK_EQ(slow_clock(33), 3);
    CHECK_EQ(slow_clock(64), 4);
}

static void the_game_timers_tick_once_every_32_frames_down_to_0(void)
{
    const GameTimers none = no_timers();
    const GameTimers timers = set_timer(&none, 3, 1);
    CHECK_EQ(tick_timers(&timers, 31).t[3], 1);
    CHECK_EQ(tick_timers(&timers, 32).t[3], 0);
    CHECK_EQ(tick_timers(&none, 64).t[0], 0);
}

int main(void)
{
    RUN(the_slow_clock_steps_twice_every_32_frames);
    RUN(the_game_timers_tick_once_every_32_frames_down_to_0);
    return check_report();
}
