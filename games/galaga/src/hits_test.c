/* SPDX-License-Identifier: GPL-3.0-only */
#include "bomber_timers.h"
#include "bombs.h"
#include "hits.h"
#include "objects.h"
#include "phase.h"
#include "playing.h"
#include "rom.h"
#include "score.h"
#include "sound_names.h"
#include "stage_setup.h"
#include "test/check.h"

static PlayingState state;
static HitReport report;

static void few_aliens_left_attack_nonstop_while_the_fighter_can_fire(void)
{
    const StageParameters p = stage_parameters(1, RANK_A);
    CHECK(continuous_bombing(&p, p.continuous_bombing - 1, 1));
    CHECK(!continuous_bombing(&p, p.continuous_bombing - 1, 0));
    const BomberSettings s = bomber_settings(&p, 3, 100, 1, 2);
    CHECK_EQ(s.reload[0], 2);
    CHECK_EQ(s.reload[1], 2);
    CHECK_EQ(s.reload[2], 2);
}

static void more_aliens_attack_after_the_first_30_seconds(void)
{
    const StageParameters p = stage_parameters(1, RANK_A);
    CHECK_EQ(bomber_settings(&p, 40, 0x78, 0, 1).max_bombers, 1);
    CHECK_EQ(bomber_settings(&p, 40, 0x3b, 0, 1).max_bombers, p.max_bombers_later);
}

static void with_all_40_alive_the_table_is_read_past_its_row_as_the_original(void)
{
    const StageParameters p = stage_parameters(1, RANK_A);
    const BomberSettings s = bomber_settings(&p, 40, 0x78, 0, 1);
    /* column 4 is the first byte of the next row (c_08BE) */
    CHECK_EQ(s.reload[0], BOMB_ENABLE_TABLE[32 + p.boss_timer * 4 + 4]);
    CHECK_EQ(s.bomb_bits, BOMB_ENABLE_TABLE[p.bomb_enable * 4 + 4]);
    CHECK_EQ(bomber_settings(&p, 39, 0x78, 0, 1).reload[0],
             BOMB_ENABLE_TABLE[32 + p.boss_timer * 4 + 3]);
}

static void bombs_fall_2_and_3_pixels_in_turn_drifting_by_their_rate(void)
{
    Objects objects = empty_objects();
    BombDrift drift[BOMB_COUNT];
    set_up_bombs(&objects);
    object_ref(&objects, FIRST_BOMB)->x = 100;
    object_ref(&objects, FIRST_BOMB)->y = 100;
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        drift[i].rate = 0x40;
        drift[i].accumulator = 0;
    }
    move_bombs(&objects, drift, 0);
    CHECK_EQ(object_at(&objects, FIRST_BOMB)->x, 102);
    CHECK_EQ(object_at(&objects, FIRST_BOMB)->y, 102);
    CHECK_EQ(object_at(&objects, FIRST_BOMB)->code, BOMB_CODE);
    move_bombs(&objects, drift, 1);
    CHECK_EQ(object_at(&objects, FIRST_BOMB)->x, 104);
    CHECK_EQ(object_at(&objects, FIRST_BOMB)->y, 105);
}

static void each_palette_is_worth_what_the_rom_table_says(void)
{
    int32_t counts[HIT_COUNTERS] = {0};
    CHECK_EQ(points_of(1), 150);
    CHECK_EQ(points_of(2), 80);
    CHECK_EQ(points_of(3), 50);
    CHECK_EQ(points_of(7), 500);
    CHECK_EQ(points_of(15), 100);
    counts[3] = 2;
    CHECK_EQ(hit_points(counts), 100);
}

/** A bee at (100, 200) with `disposition` and a rocket at (`x`, 200). */
static void shot_at(int32_t x, int32_t disposition)
{
    empty_playing_state(&state, STEP_RUNNING);
    state.objects = empty_objects();
    GameObject *bee = object_ref(&state.objects, 0x08);
    bee->disposition = (uint8_t)disposition;
    bee->x = 100;
    bee->y = 200;
    bee->color = 3;
    GameObject *rocket = object_ref(&state.objects, FIRST_ROCKET);
    rocket->disposition = DISPOSITION_SHOT;
    rocket->x = (uint8_t)x;
    rocket->y = 200;
    report = no_hit_report();
}

/** Hit inputs of the first stage. */
static HitInputs hit_inputs(void)
{
    const HitInputs inputs = {0, 0, 0, challenge_bonus(1)};
    return inputs;
}

static void a_rocket_within_5_pixels_hits_a_resting_bee_for_50_points(void)
{
    const HitInputs inputs = hit_inputs();
    shot_at(105, DISPOSITION_HOME);
    detect_hits(&state, &inputs, 0, &report);
    CHECK_EQ(state.hits, 1);
    CHECK_EQ(object_at(&state.objects, FIRST_ROCKET)->x, 0);
    CHECK_EQ(hit_points(report.hit_counts), 50);
    CHECK_EQ(state.sounds, SOUND_BEE_HIT);
    start_explosions(&state.objects);
    CHECK_EQ(object_at(&state.objects, 0x08)->disposition, DISPOSITION_EXPLODING);
}

static void a_flying_bee_is_worth_double(void)
{
    const HitInputs inputs = hit_inputs();
    shot_at(100, DISPOSITION_FLYING);
    detect_hits(&state, &inputs, 0, &report);
    CHECK_EQ(hit_points(report.hit_counts), 100);
    CHECK_EQ(state.flying_hits, 1);
}

static void a_rocket_6_pixels_away_misses(void)
{
    const HitInputs inputs = hit_inputs();
    shot_at(106, DISPOSITION_HOME);
    detect_hits(&state, &inputs, 0, &report);
    CHECK_EQ(state.hits, 0);
}

static void a_bomb_on_the_fighter_destroys_it_and_disappears(void)
{
    const HitInputs inputs = hit_inputs();
    shot_at(0, 0);
    state.objects = empty_objects();
    GameObject *fighter = object_ref(&state.objects, FIGHTER);
    fighter->disposition = 1;
    fighter->x = 120;
    fighter->y = 300;
    GameObject *bomb = object_ref(&state.objects, FIRST_BOMB);
    bomb->disposition = DISPOSITION_SHOT;
    bomb->x = 124;
    bomb->y = 302;
    bomb->color = BOMB_COLOR;
    detect_hits(&state, &inputs, 1, &report);
    CHECK_EQ(report.fighters_count, 1);
    CHECK_EQ(report.fighters_hit[0], FIGHTER);
    CHECK_EQ(object_at(&state.objects, FIRST_BOMB)->x, 0);
}

int main(void)
{
    RUN(few_aliens_left_attack_nonstop_while_the_fighter_can_fire);
    RUN(more_aliens_attack_after_the_first_30_seconds);
    RUN(with_all_40_alive_the_table_is_read_past_its_row_as_the_original);
    RUN(bombs_fall_2_and_3_pixels_in_turn_drifting_by_their_rate);
    RUN(each_palette_is_worth_what_the_rom_table_says);
    RUN(a_rocket_within_5_pixels_hits_a_resting_bee_for_50_points);
    RUN(a_flying_bee_is_worth_double);
    RUN(a_rocket_6_pixels_away_misses);
    RUN(a_bomb_on_the_fighter_destroys_it_and_disappears);
    return check_report();
}
