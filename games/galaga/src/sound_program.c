/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_program.h"

#include "sound_data.h"
#include "sound_effects.h"

/** Ticks of each segment of the pulsing sound (`l_0102`). */
#define PULSE_SEGMENT_TICKS 0x22

/** Restarting effects, lowest priority first: the dive, the shot and the hits. */
static const uint8_t RESTARTING[] = {EFFECT_DIVE,          EFFECT_SHOT,         EFFECT_BEE_HIT,
                                     EFFECT_BUTTERFLY_HIT, EFFECT_BOSS_DAMAGED, EFFECT_BOSS_HIT};

/** The tunes below the "shot your ship!" one, in their order (`l_0292`-`l_0324`). */
static const uint8_t TUNES[] = {EFFECT_PERFECT,       EFFECT_BADGE,  EFFECT_EXTRA_LIFE,
                                EFFECT_START_THEME,   EFFECT_UNUSED, EFFECT_NAME_ENTRY,
                                EFFECT_NAME_ENTRY_END};

/**
 * `l_00D3`: the pulsing sound of the breathing formation, on the first voice: a pitch that slides
 * up while the formation opens and down while it closes, in segments of 34 ticks.
 */
static void pulse(Work *work, int32_t sign)
{
    SoundState *sound = work->sound;
    Pulse *current = &sound->pulse;
    if (sound->triggers[EFFECT_PULSE] == 0)
        return;
    int turned = sign != current->sign;
    int32_t tick = turned ? 0 : (sound->tracks.ticks[0] + 1) & 0xff;
    int next_segment = turned || tick == PULSE_SEGMENT_TICKS;
    if (turned) {
        current->table = sign == 0xff ? PULSE_CLOSING : PULSE_OPENING;
        current->segment = 0;
    } else if (next_segment) {
        current->segment++;
    }
    int32_t start = current->pitch;
    if (next_segment) {
        PulseSegment loaded = pulse_segment(current->table, current->segment);
        current->step = (uint16_t)loaded.step;
        start = loaded.start;
    }
    current->sign = (uint8_t)sign;
    current->pitch = (uint16_t)((start + current->step) & 0xffff);
    sound->tracks.ticks[0] = (uint8_t)(next_segment ? 0 : tick);
    work->mix.frequencies[0] = (current->pitch >> 8) * 16;
    work->mix.volumes[0] = 0x0a;
    work->mix.waveforms[0] = 0;
}

/**
 * `l_0204`: the tractor beam, with a tremolo on its third voice: every 6 ticks the volume drops
 * by one, from 12 down to 3 and back to 12.
 */
static void tractor_beam(Work *work)
{
    SoundState *sound = work->sound;
    held(work, EFFECT_TRACTOR_BEAM);
    if (sound->triggers[EFFECT_TRACTOR_BEAM] == 0)
        return;
    int32_t ticks = sound->beam_ticks + 1;
    if (ticks >= 6)
        sound->beam_volume = (uint8_t)(sound->beam_volume < 4 ? 0x0c : sound->beam_volume - 1);
    sound->beam_ticks = (uint8_t)(ticks < 6 ? ticks : 0);
    set_volume(work, 2, sound->beam_volume);
}

/** `l_0241`: the fighter pulled up the beam: the first voice changes waveform every 28 ticks. */
static void beam_capture(Work *work)
{
    SoundState *sound = work->sound;
    held(work, EFFECT_BEAM_CAPTURE);
    if (sound->triggers[EFFECT_BEAM_CAPTURE] == 0)
        return;
    int32_t ticks = sound->pull_ticks + 1;
    if (ticks == 0x1c)
        sound->pull_waveform++;
    sound->pull_ticks = (uint8_t)(ticks == 0x1c ? 0 : ticks);
    work->mix.waveforms[0] = sound->pull_waveform;
}

/** `l_0292`-`l_0324`: the tunes, below the "shot your ship!" one, which skips them all. */
static void tunes(Work *work)
{
    held(work, EFFECT_RESCUE);
    tune(work, EFFECT_CHALLENGE_START);
    if (work->sound->triggers[EFFECT_CHALLENGE_END] != 0) {
        tune(work, EFFECT_CHALLENGE_END);
        set_volume(work, 1, 9);
        set_volume(work, 2, 6);
    }
    for (uint32_t i = 0; i < sizeof TUNES; i++) {
        if (TUNES[i] == EFFECT_UNUSED)
            held(work, TUNES[i]);
        else
            tune(work, TUNES[i]);
    }
}

void tick_sound(SoundState *sound, int32_t sign, WsgVoice voices[WSG_VOICES])
{
    Work work;
    work.sound = sound;
    for (int32_t i = 0; i < SOUND_VOICES; i++) {
        work.mix.frequencies[i] = 0;
        work.mix.volumes[i] = 0;
        work.mix.waveforms[i] = sound->waveforms[i];
    }
    pulse(&work, sign);
    for (uint32_t i = 0; i < sizeof RESTARTING; i++)
        restarting(&work, RESTARTING[i]);
    tune(&work, EFFECT_BONUS_BEE);
    tractor_beam(&work);
    beam_capture(&work);
    held(&work, EFFECT_FIGHTER_CAPTURED);
    if (sound->triggers[EFFECT_CAPTURED_FIGHTER_HIT] == 0)
        tunes(&work);
    else
        tune(&work, EFFECT_CAPTURED_FIGHTER_HIT);
    tune(&work, EFFECT_COIN);
    for (int32_t i = 0; i < SOUND_VOICES; i++) {
        sound->waveforms[i] = (uint8_t)work.mix.waveforms[i];
        voices[i].frequency = (uint32_t)work.mix.frequencies[i];
        voices[i].volume = (uint8_t)(work.mix.volumes[i] & 0x0f);
        voices[i].waveform = (uint8_t)(work.mix.waveforms[i] & 7);
    }
}
