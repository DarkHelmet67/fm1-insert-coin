/* SPDX-License-Identifier: GPL-3.0-only */
#include "playing.h"

#include "bombs.h"
#include "config.h"

void empty_playing_state(PlayingState *state, int32_t step)
{
    const Phase phase = {(uint8_t)step, 0, 0, 0, 0};
    const TripleBonus no_bonus = {0, 0};
    state->frame = 0;
    state->timers = no_timers();
    state->phase = phase;
    state->tasks = 0;
    state->objects = empty_objects();
    set_up_bombs(&state->objects);
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        state->slots.list[i] = free_slot();
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        state->bomb_drift[i].rate = 0;
        state->bomb_drift[i].accumulator = 0;
    }
    state->rocket_paths = no_paths();
    state->step_toggle = 0;
    state->dual = 0;
    state->formation = initial_formation();
    state->stars.phase = 0;
    state->stars.speed = STARS_STILL;
    state->star_control = stars_stopped();
    state->stage = 0;
    state->rank = GAME_RANK;
    state->params = stage_parameters(1, GAME_RANK);
    state->looks = no_looks();
    state->launcher = idle_launcher();
    state->entry_bomb_bits = 0;
    state->bomb_interval = 0x14;
    state->attacks = new_attacks();
    state->max_bombers = 0;
    state->flying_hits = 0;
    state->group_left = 0;
    state->capture = no_capture();
    state->triple_left = 0;
    state->triple_bonus = no_bonus;
    state->enemies_enabled = 0;
    state->waves_enabled = 0;
    state->sway_stops = 0;
    state->alive = 0;
    state->counting = 0;
    state->restart = 0;
    state->reserve = START_RESERVE;
    state->badges = 0;
    state->messages = no_messages();
    state->score = 0;
    state->next_bonus = EXTRA_LIFE_FIRST;
    state->shots = 0;
    state->hits = 0;
    state->sounds = 0;
}

void play(PlayingState *state, uint32_t sound)
{
    state->sounds |= sound;
}

int32_t flying_count(const PlayingState *state)
{
    int32_t count = 0;
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        count += state->slots.list[i].used;
    return count;
}

int task_on(const PlayingState *state, int32_t tasks)
{
    return (state->tasks & tasks) == tasks;
}

void set_tasks(PlayingState *state, int32_t on, int32_t off)
{
    state->tasks = (uint8_t)((state->tasks | on) & ~off);
}
