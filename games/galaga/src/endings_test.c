/* SPDX-License-Identifier: GPL-3.0-only */
#include <string.h>

#include "endings.h"
#include "extra_lives.h"
#include "phase.h"
#include "playing.h"
#include "sound_names.h"
#include "test/check.h"

static PlayingState state;

static void the_hit_miss_ratio_has_one_rounded_decimal_and_no_leading_zeros(void)
{
    char out[16];
    CHECK(strcmp(hit_ratio(33, 97, out), "34.0 %") == 0);
    CHECK(strcmp(hit_ratio(2, 3, out), "66.7 %") == 0);
    CHECK(strcmp(hit_ratio(1, 400, out), ".3 %") == 0);
    CHECK(strcmp(hit_ratio(0, 0, out), ".0 %") == 0);
    CHECK(strcmp(hit_ratio(5, 5, out), "100.0 %") == 0);
}

static void extra_fighters_come_at_20000_70000_then_every_70000(void)
{
    CHECK_EQ(next_bonus_after(2), 7);
    CHECK_EQ(next_bonus_after(7), 14);
    CHECK_EQ(next_bonus_after(21), 28);
    empty_playing_state(&state, STEP_RUNNING);
    const int32_t reserve = state.reserve;
    state.score = 19950;
    add_points(&state, 100);
    CHECK_EQ(state.score, 20050);
    CHECK_EQ(state.reserve, reserve + 1);
    CHECK_EQ(state.next_bonus, 7);
    CHECK(state.sounds & SOUND_EXTRA_LIFE);
    add_points(&state, 100);
    CHECK_EQ(state.reserve, reserve + 1);
}

/** Runs every line of ending `name`. */
static void run_script(int32_t name)
{
    for (int32_t line = 0; line < script_length(name); line++)
        run_script_line(&state, name, line);
}

static void challenge_results_give_100_points_per_hit(void)
{
    empty_playing_state(&state, STEP_RUNNING);
    state.flying_hits = 25;
    run_script(SCRIPT_CHALLENGE);
    CHECK_EQ(state.score, 2500);
}

static void a_perfect_stage_gives_10000_points(void)
{
    empty_playing_state(&state, STEP_RUNNING);
    state.flying_hits = 40;
    run_script_line(&state, SCRIPT_PERFECT, 0);
    CHECK_EQ(state.sounds, SOUND_PERFECT);
    for (int32_t line = 1; line < script_length(SCRIPT_PERFECT); line++)
        run_script_line(&state, SCRIPT_PERFECT, line);
    CHECK_EQ(state.score, 10000);
}

int main(void)
{
    RUN(the_hit_miss_ratio_has_one_rounded_decimal_and_no_leading_zeros);
    RUN(extra_fighters_come_at_20000_70000_then_every_70000);
    RUN(challenge_results_give_100_points_per_hit);
    RUN(a_perfect_stage_gives_10000_points);
    return check_report();
}
