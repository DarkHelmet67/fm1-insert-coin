/* SPDX-License-Identifier: GPL-3.0-only */
#include "phase.h"
#include "playing.h"
#include "sound_commands.h"
#include "sound_names.h"
#include "sound_state.h"
#include "test/check.h"

static PlayingState playing;

/** A stage being played with 40 aliens. */
static void set_up(void)
{
    empty_playing_state(&playing, STEP_RUNNING);
    playing.alive = 40;
}

/** Whether `commands` write `value` to `effect`. */
static int writes(const SoundCommands *commands, uint8_t effect, uint8_t value)
{
    for (int32_t i = 0; i < commands->count; i++)
        if (commands->writes[i].effect == effect && commands->writes[i].value == value)
            return 1;
    return 0;
}

static void the_sign_follows_the_breathing_counter_one_step_late(void)
{
    CHECK_EQ(formation_sign(0x05), 1);
    CHECK_EQ(formation_sign(0xa0), 1);
    CHECK_EQ(formation_sign(0x9f), 0xff);
    CHECK_EQ(formation_sign(0x00), 0xff);
}

static void the_sounds_of_the_frame_become_writes_the_explosion_a_noise(void)
{
    SoundCommands commands;
    set_up();
    playing.sounds = SOUND_SHOT | SOUND_FIGHTER_EXPLOSION;
    sound_commands(&playing, &commands);
    CHECK(writes(&commands, EFFECT_SHOT, 1));
    CHECK(commands.explosion);
}

static void the_tractor_beam_hums_while_the_beam_is_open(void)
{
    SoundCommands commands;
    set_up();
    sound_commands(&playing, &commands);
    CHECK(writes(&commands, EFFECT_TRACTOR_BEAM, 0));
    playing.capture.step = 0x05;
    playing.capture.tasks = CAPTURE_BEAM;
    sound_commands(&playing, &commands);
    CHECK(writes(&commands, EFFECT_TRACTOR_BEAM, 1));
}

static void the_pulsing_sound_stops_when_the_stage_is_cleared(void)
{
    SoundCommands commands;
    set_up();
    sound_commands(&playing, &commands);
    CHECK(!writes(&commands, EFFECT_PULSE, 0));
    playing.alive = 0;
    sound_commands(&playing, &commands);
    CHECK(writes(&commands, EFFECT_PULSE, 0));
}

static void the_attract_screen_asks_for_nothing(void)
{
    SoundCommands commands;
    no_sound_commands(&commands);
    CHECK_EQ(commands.count, 0);
    CHECK(!commands.explosion);
    CHECK_EQ(commands.sign, 1);
}

int main(void)
{
    RUN(the_sign_follows_the_breathing_counter_one_step_late);
    RUN(the_sounds_of_the_frame_become_writes_the_explosion_a_noise);
    RUN(the_tractor_beam_hums_while_the_beam_is_open);
    RUN(the_pulsing_sound_stops_when_the_stage_is_cleared);
    RUN(the_attract_screen_asks_for_nothing);
    return check_report();
}
