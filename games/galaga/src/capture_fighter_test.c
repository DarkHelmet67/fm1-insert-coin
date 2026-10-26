/* SPDX-License-Identifier: GPL-3.0-only */
#include <string.h>

#include "capture_fighter.h"
#include "fighter.h"
#include "objects.h"
#include "phase.h"
#include "playing.h"
#include "program.h"
#include "sound_commands.h"
#include "sound_state.h"
#include "test/check.h"
#include "update_playing.h"

static PlayingState state;

/** What the simulated game went through, and when. */
static int caught_beam_capture, carried_text, held_home, rescue_sound, joined;
static int32_t held_color, joined_x;

/** Whether `commands` write `value` to `effect`. */
static int writes(const SoundCommands *commands, uint8_t effect, uint8_t value)
{
    for (int32_t i = 0; i < commands->count; i++)
        if (commands->writes[i].effect == effect && commands->writes[i].value == value)
            return 1;
    return 0;
}

/** Whether "FIGHTER CAPTURED" is on the screen. */
static int fighter_captured_shown(void)
{
    for (int32_t i = 0; i < state.messages.count; i++)
        if (strcmp(state.messages.list[i].text, "FIGHTER CAPTURED") == 0)
            return 1;
    return 0;
}

/** Notes the first time each step of a capture and a rescue happens. */
static void watch(int first[5])
{
    SoundCommands commands;
    sound_commands(&state, &commands);
    if (!first[0] && (state.capture.tasks & CAPTURE_PULL)) {
        first[0] = 1;
        caught_beam_capture = writes(&commands, EFFECT_BEAM_CAPTURE, 1);
    }
    if (!first[1] && (state.capture.tasks & CAPTURE_CARRY) && !state.capture.show_text) {
        first[1] = 1;
        carried_text = fighter_captured_shown();
    }
    if (!first[2] && state.restart && state.capture.busy) {
        first[2] = 1;
        held_home = object_at(&state.objects, 2)->disposition == DISPOSITION_HOME;
        held_color = object_at(&state.objects, 2)->color;
    }
    if (!first[3] && (state.capture.tasks & CAPTURE_RESCUE)) {
        first[3] = 1;
        rescue_sound = writes(&commands, EFFECT_RESCUE, 1);
    }
    if (!first[4] && state.dual) {
        first[4] = 1;
        joined = 1;
        joined_x = object_at(&state.objects, FIGHTER)->x;
    }
}

/**
 * A game where the fighter cannot be hit and stays in the middle; it starts firing at frame
 * 4000, after the capture.
 */
static void simulate(void)
{
    int first[5] = {0, 0, 0, 0, 0};
    empty_playing_state(&state, STEP_INTRO);
    start_intro(&state);
    for (int32_t f = 0; f < 7100; f++) {
        const Controls controls = {0, (uint8_t)(f % 40 == 0 && f > 4000), 0};
        set_tasks(&state, 0, TASK_FIGHTER_HITS);
        update_playing(&state, &controls);
        watch(first);
    }
}

static void a_full_turn_takes_24_steps_and_stops_upright(void)
{
    GameObject fighter = no_object();
    fighter.code = FIGHTER_UPRIGHT;
    GameObject turned = fighter;
    spin(&turned, 1);
    CHECK_EQ(turned.code, 5);
    CHECK(turned.flip_x && !turned.flip_y);
    for (int i = 1; i < 24; i++)
        spin(&turned, 1);
    CHECK_EQ(turned.code, FIGHTER_UPRIGHT);
    CHECK(!turned.flip_x && !turned.flip_y);
    CHECK(spin(&fighter, 0));
}

static void a_boss_catches_the_fighter_and_takes_it_home(void)
{
    simulate();
    CHECK(caught_beam_capture);
    CHECK(carried_text);
    CHECK(held_home);
    CHECK_EQ(held_color, 7);
}

static void shooting_the_diving_boss_frees_the_fighter_a_double_fighter(void)
{
    CHECK(rescue_sound);
    CHECK(joined);
    CHECK_EQ(joined_x, 0x71);
}

int main(void)
{
    RUN(a_full_turn_takes_24_steps_and_stops_upright);
    RUN(a_boss_catches_the_fighter_and_takes_it_home);
    RUN(shooting_the_diving_boss_frees_the_fighter_a_double_fighter);
    return check_report();
}
