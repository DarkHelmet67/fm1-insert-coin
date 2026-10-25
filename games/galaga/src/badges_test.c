/* SPDX-License-Identifier: GPL-3.0-only */
#include "badges.h"
#include "test/check.h"

static void stages_count_flags_of_50_30_20_10_5_and_1(void)
{
    CHECK_EQ(stage_flag_count(1), 1u);
    CHECK_EQ(stage_flag_count(7), 3u);   /* 5, 1, 1 */
    CHECK_EQ(stage_flag_count(48), 6u);  /* 30, 10, 5, 1, 1, 1 */
    CHECK_EQ(stage_flag_count(100), 2u); /* 50, 50 */
}

static void one_five_and_fifty_are_in_palette_1_the_others_in_2(void)
{
    BadgeColumn columns[2 * MAX_FLAGS];
    const uint32_t count = badge_columns(15, 2, columns); /* 10 (two columns) and 5 (one) */
    CHECK_EQ(count, 3u);
    CHECK_EQ(columns[0].palette, 2);
    CHECK_EQ(columns[1].palette, 2);
    CHECK_EQ(columns[2].palette, 1);
}

static void the_first_flag_of_stage_1_is_character_36(void)
{
    BadgeColumn columns[2 * MAX_FLAGS];
    CHECK_EQ(badge_columns(1, 1, columns), 1u);
    CHECK_EQ(columns[0].top, 0x36);
}

int main(void)
{
    RUN(stages_count_flags_of_50_30_20_10_5_and_1);
    RUN(one_five_and_fifty_are_in_palette_1_the_others_in_2);
    RUN(the_first_flag_of_stage_1_is_character_36);
    return check_report();
}
