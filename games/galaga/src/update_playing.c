/* SPDX-License-Identifier: GPL-3.0-only */
#include "update_playing.h"

#include "capture_fighter.h"
#include "clone_attack.h"
#include "combat.h"
#include "fighter.h"
#include "flight.h"
#include "object_states.h"
#include "program.h"
#include "rescue.h"
#include "tractor_beam.h"

/** `f_1D76` and the star hardware: speed, then one frame of scrolling. */
static void update_stars(PlayingState *state)
{
    state->stars.speed = control_stars(&state->star_control);
    state->stars.phase = advance_stars(state->stars.phase, state->stars.speed);
}

/** `f_2916`: the next alien of the attack waves. */
static void run_waves(PlayingState *state)
{
    LauncherInputs inputs;
    inputs.frame = state->frame;
    inputs.enabled = state->waves_enabled;
    inputs.flying = flying_count(state);
    inputs.challenge = is_challenge_stage(state->stage);
    inputs.timer = state->timers.t[0];
    inputs.looks = &state->looks;
    inputs.entry_bomb_bits = state->entry_bomb_bits;
    LauncherReport report = run_launcher(&state->launcher, &state->slots, &state->objects, &inputs);
    if (report.timer >= 0)
        state->timers = set_timer(&state->timers, 0, report.timer);
    if (report.group_reset)
        state->group_left = 8;
    if (report.completed) {
        state->sway_stops = 1;
        set_tasks(state, TASK_ATTACKS | TASK_CLONES, 0);
    }
}

/** `f_2A90` or `f_1DE6`: the grid sways while the waves enter, then breathes. */
static void move_formation(PlayingState *state)
{
    if (task_on(state, TASK_SWAY)) {
        if (state->alive == 0 && !state->launcher.running) {
            set_tasks(state, 0, TASK_SWAY);
            return;
        }
        SwayResult sway = sway_formation(&state->formation, state->frame, state->sway_stops);
        state->formation = sway.formation;
        if (sway.settled) {
            set_tasks(state, TASK_BREATHING, TASK_SWAY);
            play(state, SOUND_PULSE);
        }
    } else if (task_on(state, TASK_BREATHING)) {
        state->formation = breathe_formation(&state->formation, state->frame);
    }
}

/** What the flight programs read about the game. */
static FlightInputs flight_inputs(const PlayingState *state)
{
    FlightInputs inputs;
    inputs.frame = state->frame;
    inputs.fighter_x = object_at(&state->objects, FIGHTER)->x;
    inputs.continuous_bombing = is_continuous(state);
    inputs.rescue_running = (state->capture.tasks & CAPTURE_RESCUE) != 0;
    inputs.harder_entries = state->params.harder_entries;
    inputs.harder_attacks = state->params.harder_attacks;
    inputs.bomb_enable_bits = current_settings(state).bomb_bits;
    inputs.bomb_interval = state->bomb_interval;
    inputs.bombs_allowed = task_on(state, TASK_FIRE) && state->timers.t[1] == 0;
    inputs.formation = &state->formation;
    inputs.bonus_bee_look = state->attacks.bonus_bee_look;
    return inputs;
}

/**
 * What the flight programs report: the capturing Boss starts its dive (`case_0A53`), the beam
 * will open above the fighter; the bonus bee is back home (`l_0E2A`), as a plain alien.
 */
static void handle_event(PlayingState *state, const FlightEvent *event)
{
    if (event->kind == EVENT_BONUS_BEE_HOME) {
        state->attacks.bonus_bee = 1;
    } else if (event->kind == EVENT_CAPTURE_DIVE) {
        state->capture.slot = event->slot;
        state->capture.beam_x = event->target_x;
        state->capture.step = 0;
        set_capture_tasks(&state->capture, CAPTURE_APPROACH, 0);
    }
}

/** `f_08D3` on the second processor: every flying alien follows its program. */
static void fly_aliens(PlayingState *state)
{
    FlightEvents events;
    FlightWorld world = {&state->slots, &state->objects, state->bomb_drift, &events};
    const FlightInputs inputs = flight_inputs(state);
    int dive = 0;
    events.count = 0;
    run_flights(&world, &inputs);
    for (int32_t i = 0; i < events.count; i++) {
        handle_event(state, &events.list[i]);
        dive |= events.list[i].kind == EVENT_DIVE_SOUND;
    }
    if (dive)
        play(state, SOUND_DIVE);
}

/** `f_23DD`: the object manager. */
static void manage_objects(PlayingState *state)
{
    const ObjectWorld world = {&state->objects, &state->slots, &state->counting, &state->alive};
    ObjectInputs inputs;
    inputs.frame = state->frame;
    inputs.formation = &state->formation;
    inputs.enemies_enabled = state->enemies_enabled;
    update_object_states(&world, &inputs);
}

/** The fighter's part of a frame: move, fire, move the rockets. */
static void update_fighter(PlayingState *state, const Controls *controls)
{
    int32_t direction = task_on(state, TASK_STICK) ? controls->direction : 0;
    int fired = 0;
    move_fighter(&state->objects, &state->step_toggle, direction, state->dual);
    if (task_on(state, TASK_FIRE) && controls->fire)
        fired = fire_rocket(&state->objects, &state->rocket_paths, state->dual);
    move_rockets(&state->objects, &state->rocket_paths);
    if (fired) {
        state->shots++;
        play(state, SOUND_SHOT);
    }
}

void update_playing(PlayingState *state, const Controls *controls)
{
    state->frame++;
    state->sounds = 0;
    state->timers = tick_timers(&state->timers, state->frame);
    run_program(state);
    update_stars(state);
    run_waves(state);
    move_formation(state);
    clone_attack(state);
    attack(state);
    carry_fighter(state);
    run_beam(state);
    approach(state);
    pull_fighter(state);
    run_rescue(state);
    fly_aliens(state);
    manage_objects(state);
    explode(state);
    update_fighter(state, controls);
    fall_bombs(state);
    collide(state);
}
