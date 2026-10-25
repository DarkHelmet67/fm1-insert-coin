/* SPDX-License-Identifier: GPL-3.0-only */
#include "rescue.h"

#include "capture_fighter.h"
#include "fighter.h"

/** Where the rescued fighter lands: the middle of the screen, then the bottom row. */
#define LANDING_X 0x80
#define MAIN_X (LANDING_X - DUAL_SPACING)

/** The steps of the rescue (Capture.step). */
enum { RESCUE_START, RESCUE_SPIN, RESCUE_LAND, RESCUE_JOIN };

/** `l_20BF`: the rescue is over. */
static void end_rescue(PlayingState *state)
{
    set_capture_tasks(&state->capture, 0, CAPTURE_RESCUE);
}

/** `l_2097`: the two fighters join; the rescued one becomes the second, or the only one. */
static void join(PlayingState *state)
{
    GameObject *rescued = object_ref(&state->objects, state->capture.object);
    int main_upright = object_at(&state->objects, FIGHTER)->code == FIGHTER_UPRIGHT;
    int32_t y = rescued->y;
    rescued->x = 0;
    rescued->disposition = DISPOSITION_INACTIVE;
    GameObject *target = object_ref(&state->objects, main_upright ? FIGHTER_2 : FIGHTER);
    target->x = LANDING_X;
    target->y = (uint16_t)y;
    target->code = FIGHTER_UPRIGHT;
    target->color = FIGHTER_COLOR;
    target->flip_x = 0;
    target->flip_y = 0;
    target->big = 0;
    target->disposition = DISPOSITION_INACTIVE;
    state->dual = (uint8_t)main_upright;
    if (!main_upright)
        state->capture.busy = 0;
    set_tasks(state, TASK_STICK | TASK_FIRE | TASK_FIGHTER_HITS, 0);
    state->star_control.ship_on_screen = 1;
    end_rescue(state);
}

/** `l_2026`-`l_2075`: the rescued fighter glides down while the player's moves aside. */
static void land(PlayingState *state)
{
    GameObject *rescued = object_ref(&state->objects, state->capture.object);
    if (rescued->x != LANDING_X)
        rescued->x = (uint8_t)toward(rescued->x, LANDING_X);
    else if (rescued->y == FIGHTER_START_Y)
        state->capture.step = RESCUE_JOIN;
    else
        rescued->y++;
    GameObject *main = object_ref(&state->objects, FIGHTER);
    if (main->code == FIGHTER_UPRIGHT && main->x != MAIN_X)
        main->x = (uint8_t)toward(main->x, MAIN_X);
    else if (state->capture.step == RESCUE_JOIN)
        join(state);
}

/**
 * `l_20D1`: the rescued fighter spins while aliens still fly; when it stops, the player's
 * fighter stops too until the two have joined.
 */
static void spin_in_place(PlayingState *state)
{
    int keep_spinning = state->timers.t[1] != 0 || flying_count(state) != 0;
    if (!spin(object_ref(&state->objects, state->capture.object), keep_spinning))
        return;
    state->capture.step = RESCUE_LAND;
    set_tasks(state, 0, TASK_STICK | TASK_FIRE | TASK_FIGHTER_HITS);
}

void run_rescue(PlayingState *state)
{
    Capture *capture = &state->capture;
    if ((capture->tasks & CAPTURE_RESCUE) == 0)
        return;
    if (object_at(&state->objects, capture->object)->disposition != 0) {
        end_rescue(state);
        return;
    }
    switch (capture->step) {
    case RESCUE_START:
        capture->step = RESCUE_SPIN;
        state->timers = set_timer(&state->timers, 1, 2);
        return;
    case RESCUE_SPIN:
        spin_in_place(state);
        return;
    default:
        land(state);
        return;
    }
}
