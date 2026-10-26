/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_board.h"
#include "test/check.h"

/** Whether any voice of `frame` sounds. */
static int sounds(const WsgFrame *frame)
{
    for (int t = 0; t < WSG_TICKS; t++)
        for (int v = 0; v < WSG_VOICES; v++)
            if (frame->tick[t][v].volume != 0 && frame->tick[t][v].frequency != 0)
                return 1;
    return 0;
}

static void a_silent_board_stays_silent(void)
{
    SoundState sound = new_sound_state();
    SoundCommands commands;
    WsgFrame frame;
    no_sound_commands(&commands);
    sound_frame(&sound, &commands, &frame);
    CHECK(!sounds(&frame));
}

static void a_shot_written_to_its_trigger_plays(void)
{
    SoundState sound = new_sound_state();
    SoundCommands commands;
    WsgFrame frame;
    no_sound_commands(&commands);
    commands.writes[0].effect = EFFECT_SHOT;
    commands.writes[0].value = 1;
    commands.count = 1;
    sound_frame(&sound, &commands, &frame);
    CHECK_EQ(sound.triggers[EFFECT_SHOT] | sound.playing[EFFECT_SHOT], 1);
    CHECK(sounds(&frame));
}

int main(void)
{
    RUN(a_silent_board_stays_silent);
    RUN(a_shot_written_to_its_trigger_plays);
    return check_report();
}
