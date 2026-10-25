/* SPDX-License-Identifier: GPL-3.0-only */
#include "tractor_beam.h"

#include "attacks.h"

/** The flight slot of the capturing Boss, or NULL when it is free. */
static FlightSlot *boss_slot(PlayingState *state)
{
    int32_t index = state->capture.slot;
    if (index >= SLOT_COUNT || !state->slots.list[index].used)
        return 0;
    return &state->slots.list[index];
}

/** Whether the capturing Boss is still diving (not shot, not home). */
static int boss_diving(const PlayingState *state)
{
    return object_at(&state->objects, state->capture.object)->disposition == DISPOSITION_FLYING;
}

void approach(PlayingState *state)
{
    Capture *capture = &state->capture;
    if ((capture->tasks & CAPTURE_APPROACH) == 0)
        return;
    if (!boss_diving(state)) {
        capture->busy = 0;
        set_capture_tasks(capture, 0, CAPTURE_APPROACH);
        return;
    }
    FlightSlot *slot = boss_slot(state);
    if (!slot || slot->speed_odd != 0 || slot->speed_even != 0)
        return;
    int32_t turn = (slot->angle & 0x100) != 0 ? -0x0c & 0xff : 0x0c;
    int upright = ((((slot->angle >> 1) & 0xff) - 0x78) & 0xff) < 0x10;
    if (!upright) {
        slot->turn = (uint8_t)turn;
        return;
    }
    capture->beam_speed = state->params.beam_speed;
    capture->step = 0;
    capture->connected = 0;
    capture->countdown = 1;
    capture->pulling = 1;
    set_capture_tasks(capture, CAPTURE_BEAM, CAPTURE_APPROACH);
    slot->turn = 0;
}

/** `l_2327`: the Boss was shot while beaming: the beam closes fast, the fighter is free. */
static void boss_shot(PlayingState *state)
{
    state->capture.beam_speed = 3;
    state->capture.step = 0x80;
    state->capture.pulling = 0;
    state->capture.countdown = 1;
    state->star_control.backwards = 0;
    set_tasks(state, TASK_STICK, 0);
}

/** `l_22E3`: the beam is gone; the Boss flies on, or goes home with the fighter. */
static void beam_ends(PlayingState *state)
{
    Capture *capture = &state->capture;
    FlightSlot *slot = boss_slot(state);
    set_capture_tasks(capture, 0, CAPTURE_BEAM);
    if (!capture->connected) {
        capture->busy = 0;
        capture->object = 1;
        if (slot)
            slot->timer = 1;
        return;
    }
    capture->show_text = 1;
    set_capture_tasks(capture, CAPTURE_CARRY, 0);
    state->star_control.backwards = 0;
    if (slot)
        slot->pointer = PROGRAM_CAPTURE_HOME;
}

/** One step of a beam that is opening, holding or closing (`l_226A`). */
static void beam_step(PlayingState *state)
{
    Capture *capture = &state->capture;
    capture->countdown = capture->beam_speed;
    int32_t step = capture->step + 1;
    int32_t row = step & 0x0f;
    if ((capture->step & 0x80) != 0) {
        /* Shot while beaming: the rows are erased from the top. */
        capture->step = (uint8_t)step;
        if (row == 0x0b) {
            capture->busy = 0;
            set_capture_tasks(capture, 0, CAPTURE_BEAM);
        } else {
            set_beam_row(capture, row, 0);
        }
        return;
    }
    /* The Boss hovers while the beam is on: its instruction never runs out. */
    FlightSlot *slot = boss_slot(state);
    if (slot)
        slot->timer = 0xff;
    if (row != 0x0b) {
        capture->step = (uint8_t)step;
        if ((step & 0x40) != 0)
            set_beam_row(capture, 0x0b - row, 0);
        else
            set_beam_row(capture, row, 1);
    } else if ((step & 0x40) == 0) {
        capture->step = 0x40;
        capture->countdown = 0x40;
    } else if (capture->connected && (step & 0x20) == 0) {
        capture->step = 0x68;
    } else {
        capture->step = (uint8_t)step;
        beam_ends(state);
    }
}

void try_to_catch(PlayingState *state)
{
    Capture *capture = &state->capture;
    if (capture->step != 0x40)
        return;
    int32_t distance = (capture->beam_x - object_at(&state->objects, FIGHTER)->x + 0x1b) & 0xff;
    if (distance >= 0x36 || !task_on(state, TASK_STICK) || state->restart)
        return;
    capture->beam_speed = 0x0a;
    set_capture_tasks(capture, CAPTURE_PULL, 0);
    set_tasks(state, 0, TASK_STICK | TASK_FIGHTER_HITS);
    state->star_control.backwards = 1;
}

void run_beam(PlayingState *state)
{
    Capture *capture = &state->capture;
    if ((capture->tasks & CAPTURE_BEAM) == 0)
        return;
    if ((capture->step & 0x80) == 0 && !boss_diving(state)) {
        boss_shot(state);
        return;
    }
    capture->countdown = (uint8_t)(capture->countdown - 1);
    if (capture->countdown != 0) {
        try_to_catch(state);
        return;
    }
    beam_step(state);
}
