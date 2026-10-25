/* SPDX-License-Identifier: GPL-3.0-only */
#include "capture_fighter.h"

/** Sprite y of the fighter at the bottom of the screen, and where it meets the Boss. */
#define BOTTOM_Y 0x129
#define NO_FIRE_Y 0xe6
#define CONNECTED_Y 0xe0

/** The red palette of a captured fighter. */
#define CAPTURED_COLOR 7

/** Steps of 1 pixel that bring the captured fighter above its Boss, once home. */
#define RISE_STEPS 0x24

int spin(GameObject *object, int keep_spinning)
{
    int32_t ctrl = (object->flip_y ? 1 : 0) | (object->flip_x ? 2 : 0);
    int32_t pose = object->code & 7;
    int32_t quarter = ctrl ^ (ctrl >> 1);
    if (pose == 6 && quarter == 0 && !keep_spinning)
        return 1;
    /* Counts up in even quarters and down in odd ones, moving to the next quarter at the ends. */
    while ((quarter & 1) == 0 ? pose == 6 : pose == 0)
        quarter = quarter == 0 ? 3 : quarter - 1;
    int32_t flips = (quarter & 2) != 0 ? quarter ^ 1 : quarter;
    object->code = (uint8_t)(object->code + ((quarter & 1) == 0 ? 1 : -1));
    object->flip_x = (flips & 2) != 0;
    object->flip_y = (flips & 1) != 0;
    return 0;
}

int32_t toward(int32_t value, int32_t target)
{
    if (value == target)
        return value;
    return value + (((target - value) & 0x80) == 0 ? 1 : -1);
}

/** `l_217F`: the pull is over; the fighter can be hit again. */
static void end_pull(PlayingState *state)
{
    set_capture_tasks(&state->capture, 0, CAPTURE_PULL);
    set_tasks(state, TASK_FIGHTER_HITS, 0);
}

/** `l_215D`: the fighter freed by a shot Boss drops back to the bottom, still spinning. */
static void drop_back(PlayingState *state, int upright)
{
    GameObject *fighter = object_ref(&state->objects, FIGHTER);
    if (fighter->y != BOTTOM_Y)
        fighter->y++;
    else if (upright)
        end_pull(state);
}

void pull_fighter(PlayingState *state)
{
    Capture *capture = &state->capture;
    if ((capture->tasks & CAPTURE_PULL) == 0)
        return;
    GameObject *fighter = object_ref(&state->objects, FIGHTER);
    int upright = spin(fighter, capture->pulling);
    if (upright && !task_on(state, TASK_FIRE)) {
        capture->connected = 1;
        end_pull(state);
        return;
    }
    if (upright || (capture->step & 0x80) != 0) {
        drop_back(state, upright);
        return;
    }
    if (!capture->pulling)
        return;
    const GameObject *boss = object_at(&state->objects, capture->object);
    fighter->x = (uint8_t)toward(fighter->x, boss->x);
    fighter->y--;
    if (fighter->y == NO_FIRE_Y) {
        set_tasks(state, 0, TASK_FIRE);
    } else if (fighter->y == CONNECTED_Y) {
        fighter->color = CAPTURED_COLOR;
        capture->pulling = 0;
    }
}

/** `l_19D2`: the fighter becomes the captured fighter of the Boss, above the text. */
static void become_captured(PlayingState *state)
{
    GameObject *captured = object_ref(&state->objects, state->capture.object & 7);
    GameObject *fighter = object_ref(&state->objects, FIGHTER);
    const Message text = message_of(MESSAGE_FIGHTER_CAPTURED);
    captured->x = fighter->x;
    captured->y = fighter->y;
    captured->flip_x = fighter->flip_x;
    captured->flip_y = fighter->flip_y;
    captured->code = 7;
    captured->color = CAPTURED_COLOR;
    fighter->x = 0;
    state->messages = messages_show(&state->messages, &text);
    state->timers = set_timer(&state->timers, 1, 6);
    state->capture.step = 0;
    state->capture.show_text = 0;
}

/** `l_1A3F`: the Boss is home; the captured fighter climbs above it and rests. */
static void join_formation(PlayingState *state)
{
    Capture *capture = &state->capture;
    GameObject *captured = object_ref(&state->objects, capture->object & 7);
    int32_t step = capture->step;
    if (step == 0)
        captured->code = 6;
    capture->step = (uint8_t)(step + 1);
    if (step != RISE_STEPS) {
        captured->y--;
        return;
    }
    captured->disposition = DISPOSITION_HOME;
    capture->object = 1;
    set_capture_tasks(capture, 0, CAPTURE_CARRY);
    state->star_control.ship_on_screen = 1;
    state->restart = 1;
}

void carry_fighter(PlayingState *state)
{
    Capture *capture = &state->capture;
    if ((capture->tasks & CAPTURE_CARRY) == 0)
        return;
    if (capture->show_text) {
        become_captured(state);
        return;
    }
    int32_t timer = state->timers.t[1];
    if (timer != 0) {
        /* While the text shows the Boss waits, its instruction kept from running out. At 4 the
         * timer skips to 3, which the tune of the captured fighter waits for (`l_19C7`). */
        if (timer == 4)
            state->timers = set_timer(&state->timers, 1, 3);
        if (capture->slot < SLOT_COUNT && state->slots.list[capture->slot].used)
            state->slots.list[capture->slot].timer = 4;
        return;
    }
    const GameObject *boss = object_at(&state->objects, capture->object);
    state->messages =
        messages_erase_row(&state->messages, message_of(MESSAGE_FIGHTER_CAPTURED).row);
    if (boss->disposition != DISPOSITION_FLYING) {
        join_formation(state);
        return;
    }
    GameObject *captured = object_ref(&state->objects, capture->object & 7);
    captured->x = boss->x;
    captured->y = (uint16_t)((boss->y + 0x10) & 0x1ff);
}
