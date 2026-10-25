/* SPDX-License-Identifier: GPL-3.0-only */
#include "combat.h"

#include "attacks.h"
#include "bombs.h"
#include "extra_lives.h"
#include "hits.h"

int is_continuous(const PlayingState *state)
{
    return continuous_bombing(&state->params, state->alive, task_on(state, TASK_FIRE));
}

BomberSettings current_settings(const PlayingState *state)
{
    return bomber_settings(&state->params, state->alive, state->timers.t[2], is_continuous(state),
                           state->max_bombers);
}

void attack(PlayingState *state)
{
    if (!task_on(state, TASK_ATTACKS))
        return;
    AttackWorld world = {&state->attacks, &state->objects, &state->slots, &state->capture, 0};
    AttackInputs inputs;
    inputs.frame = state->frame;
    inputs.settings = current_settings(state);
    inputs.flying = flying_count(state);
    inputs.enemies_enabled = state->enemies_enabled;
    inputs.fire_enabled = task_on(state, TASK_FIRE);
    inputs.rescue_running = (state->capture.tasks & CAPTURE_RESCUE) != 0;
    run_attacks(&world, &inputs);
    state->max_bombers = inputs.settings.max_bombers;
    if (world.dived)
        play(state, SOUND_DIVE);
}

void explode(PlayingState *state)
{
    start_explosions(&state->objects);
}

void fall_bombs(PlayingState *state)
{
    move_bombs(&state->objects, state->bomb_drift, state->frame);
}

/**
 * `hitd_fghtr_hit`: the fighter explodes where it was. The joystick and the fire button stop,
 * the stars stop, and the main program will restart the stage.
 */
static void lose_fighter(PlayingState *state)
{
    const GameObject *fighter = object_at(&state->objects, FIGHTER);
    explode_fighter(&state->objects, FIGHTER, fighter->x, fighter->y);
    set_tasks(state, 0, TASK_STICK | TASK_FIRE | TASK_FIGHTER_HITS);
    state->star_control.ship_on_screen = 0;
    state->restart = 1;
}

/**
 * `l_0621`: one fighter of a double fighter is hit. The other flies on alone, always as the main
 * fighter: when the main one is hit, the second takes its place and its own sprite shows the
 * explosion. A new capture becomes possible.
 */
static void lose_one_of_two(PlayingState *state, int32_t hit)
{
    const GameObject lost = *object_at(&state->objects, hit);
    if (hit == FIGHTER)
        object_ref(&state->objects, FIGHTER)->x = object_at(&state->objects, FIGHTER_2)->x;
    explode_fighter(&state->objects, FIGHTER_2, lost.x, lost.y);
    state->dual = 0;
    state->capture.busy = 0;
}

/** The fighters hit this frame are lost, the second of a double fighter first (`f_05EE`). */
static void lose_fighters(PlayingState *state, const HitReport *report)
{
    if (report->fighters_count == 0)
        return;
    for (int32_t i = 0; i < report->fighters_count; i++) {
        if (state->dual)
            lose_one_of_two(state, report->fighters_hit[i]);
        else
            lose_fighter(state);
    }
    play(state, SOUND_FIGHTER_EXPLOSION);
}

void collide(PlayingState *state)
{
    HitInputs inputs;
    HitReport report = no_hit_report();
    inputs.dual = state->dual;
    inputs.rescue_running = (state->capture.tasks & CAPTURE_RESCUE) != 0;
    inputs.waves_entering = state->launcher.running;
    inputs.challenge_bonus = challenge_bonus(state->stage);
    detect_hits(state, &inputs, task_on(state, TASK_STICK | TASK_FIGHTER_HITS), &report);
    if (report.quiet_timer >= 0)
        state->timers = set_timer(&state->timers, 1, report.quiet_timer);
    add_points(state, hit_points(report.hit_counts));
    lose_fighters(state, &report);
}
