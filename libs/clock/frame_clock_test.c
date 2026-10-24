/* SPDX-License-Identifier: GPL-3.0-only */
#include "clock/frame_clock.h"
#include "test/check.h"

static void no_time_no_frames(void)
{
    CHECK_EQ(frame_clock_frames(0u), 0u);
    CHECK_EQ(frame_clock_frames(16u), 0u);
}

static void a_frame_every_sixtieth_of_a_second(void)
{
    CHECK_EQ(frame_clock_frames(17u), 1u);
    CHECK_EQ(frame_clock_frames(34u), 2u);
    CHECK_EQ(frame_clock_frames(50u), 3u);
}

static void sixty_frames_a_second_with_no_drift(void)
{
    CHECK_EQ(frame_clock_frames(1000u), 60u);
    CHECK_EQ(frame_clock_frames(3600000u), 216000u);
}

static void no_overflow_near_the_end_of_the_counter(void)
{
    CHECK_EQ(frame_clock_frames(0xFFFFFFFFu), 257698037u);
}

int main(void)
{
    RUN(no_time_no_frames);
    RUN(a_frame_every_sixtieth_of_a_second);
    RUN(sixty_frames_a_second_with_no_drift);
    RUN(no_overflow_near_the_end_of_the_counter);
    return check_report();
}
