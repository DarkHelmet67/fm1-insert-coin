/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_board.h"

#include "sound_program.h"

void sound_frame(SoundState *sound, const SoundCommands *commands, WsgFrame *frame)
{
    for (int32_t i = 0; i < commands->count; i++)
        sound->triggers[commands->writes[i].effect] = commands->writes[i].value;
    for (int32_t tick = 0; tick < WSG_TICKS; tick++)
        tick_sound(sound, commands->sign, frame->tick[tick]);
}
