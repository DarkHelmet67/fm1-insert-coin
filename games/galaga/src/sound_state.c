/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_state.h"

SoundState new_sound_state(void)
{
    SoundState sound;
    for (int32_t i = 0; i < EFFECT_COUNT; i++) {
        sound.triggers[i] = 0;
        sound.playing[i] = 0;
    }
    for (int32_t i = 0; i < TRACK_COUNT; i++) {
        sound.tracks.ticks[i] = 0;
        sound.tracks.notes[i] = 0;
    }
    for (int32_t i = 0; i < SOUND_VOICES; i++)
        sound.waveforms[i] = 0;
    sound.pulse.sign = 0;
    sound.pulse.table = 0;
    sound.pulse.segment = 0;
    sound.pulse.pitch = 0;
    sound.pulse.step = 0;
    sound.beam_volume = 0;
    sound.beam_ticks = 0;
    sound.pull_waveform = 0;
    sound.pull_ticks = 0;
    return sound;
}
