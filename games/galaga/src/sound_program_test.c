/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_data.h"
#include "sound_program.h"
#include "sound_state.h"
#include "sound_tracks.h"
#include "test/check.h"

static SoundState sound;
static WsgVoice voices[WSG_VOICES];

/** The memory at power-on with the effects `effects` (`count` of them) triggered. */
static void trigger(const uint8_t *effects, int count)
{
    sound = new_sound_state();
    for (int i = 0; i < count; i++)
        sound.triggers[effects[i]] = 1;
}

/** Whether any voice of the last tick sounds. */
static int any_volume(void)
{
    return voices[0].volume || voices[1].volume || voices[2].volume;
}

static void the_effect_table_says_which_tracks_each_effect_plays(void)
{
    const EffectTracks theme = effect_tracks(EFFECT_START_THEME);
    const EffectTracks shot = effect_tracks(EFFECT_SHOT);
    CHECK_EQ(theme.first, 0x13);
    CHECK_EQ(theme.count, 3);
    CHECK_EQ(theme.voice, 0);
    CHECK_EQ(shot.first, 0x1f);
    CHECK_EQ(shot.count, 1);
    CHECK_EQ(shot.voice, 2);
    CHECK_EQ(tempo(EFFECT_START_THEME), 4);
}

static void each_octave_down_halves_the_pitch_and_a_rest_has_none(void)
{
    CHECK_EQ(note_pitch(0x00), 0x8150);
    CHECK_EQ(note_pitch(0x10), 0x8150 >> 1);
    CHECK_EQ(note_pitch(0x71), 0x8900 >> 7);
    CHECK_EQ(note_pitch(NOTE_REST), 0);
    CHECK_EQ(pulse_segment(PULSE_OPENING, 0).step, 0x0130);
    CHECK_EQ(pulse_segment(PULSE_OPENING, 0).start, 0x5b00);
}

static void the_envelope_holds_10_then_fades_one_a_tick(void)
{
    TrackHeader header = {0, 3, 0, 0};
    CHECK_EQ(envelope_volume(&header, 1), 10);
    CHECK_EQ(envelope_volume(&header, 3), 10);
    CHECK_EQ(envelope_volume(&header, 4), 9);
    CHECK_EQ(envelope_volume(&header, 12), 1);
    CHECK_EQ(envelope_volume(&header, 13), 0);
    header.envelope = 1; /* rises first */
    CHECK_EQ(envelope_volume(&header, 1), 2);
    CHECK_EQ(envelope_volume(&header, 5), 10);
    header.envelope = 2; /* falls from 14 */
    CHECK_EQ(envelope_volume(&header, 1), 14);
    header.envelope = 0;
    header.hold = 0; /* never fades */
    CHECK_EQ(envelope_volume(&header, 200), 10);
}

static void no_trigger_is_silence(void)
{
    trigger(0, 0);
    tick_sound(&sound, 1, voices);
    CHECK(!any_volume());
}

static void the_shot_plays_on_the_third_voice_and_clears_its_trigger(void)
{
    static const uint8_t SHOT[] = {EFFECT_SHOT};
    trigger(SHOT, 1);
    tick_sound(&sound, 1, voices);
    CHECK(voices[2].volume > 0);
    CHECK_EQ(sound.triggers[EFFECT_SHOT], 0);
    CHECK(sound.playing[EFFECT_SHOT]);
}

static void a_tune_plays_to_its_end_then_clears_its_trigger(void)
{
    static const uint8_t THEME[] = {EFFECT_START_THEME};
    int32_t end = -1;
    trigger(THEME, 1);
    for (int32_t tick = 0; tick < 1000 && end < 0; tick++) {
        tick_sound(&sound, 1, voices);
        if (sound.triggers[EFFECT_START_THEME] == 0)
            end = tick;
    }
    CHECK(end > 750 && end < 900); /* about seven seconds at 120 ticks a second */
    tick_sound(&sound, 1, voices);
    CHECK(!any_volume());
}

static void the_tractor_beam_trembles_between_volumes_3_and_12(void)
{
    static const uint8_t BEAM[] = {EFFECT_TRACTOR_BEAM};
    int32_t low = 15, high = 0;
    trigger(BEAM, 1);
    for (int32_t tick = 0; tick < 1000; tick++) {
        tick_sound(&sound, 1, voices);
        if (tick >= 10) {
            low = voices[2].volume < low ? voices[2].volume : low;
            high = voices[2].volume > high ? voices[2].volume : high;
        }
    }
    CHECK(any_volume());
    CHECK_EQ(low, 3);
    CHECK_EQ(high, 12);
}

static void a_later_effect_takes_the_voice_it_shares(void)
{
    static const uint8_t BOTH[] = {EFFECT_BEE_HIT, EFFECT_BOSS_HIT};
    static const uint8_t BOSS[] = {EFFECT_BOSS_HIT};
    WsgVoice both;
    trigger(BOTH, 2);
    tick_sound(&sound, 1, voices);
    both = voices[1];
    trigger(BOSS, 1);
    tick_sound(&sound, 1, voices);
    CHECK_EQ(both.frequency, voices[1].frequency);
    CHECK_EQ(both.volume, voices[1].volume);
}

/** The pitch of the first voice after 30 ticks of the pulse, and at the first. */
static void pulse_pitches(int32_t sign, uint32_t *first, uint32_t *last)
{
    static const uint8_t PULSE[] = {EFFECT_PULSE};
    trigger(PULSE, 1);
    for (int32_t tick = 0; tick < 30; tick++) {
        tick_sound(&sound, sign, voices);
        if (tick == 0)
            *first = voices[0].frequency;
    }
    *last = voices[0].frequency;
}

static void the_pulse_slides_up_while_the_formation_opens_down_while_it_closes(void)
{
    uint32_t first, last;
    pulse_pitches(1, &first, &last);
    CHECK(last > first);
    pulse_pitches(0xff, &first, &last);
    CHECK(last < first);
}

static void shot_your_ship_clears_the_other_sounds_but_not_the_extra_fighter(void)
{
    static const uint8_t THREE[] = {EFFECT_RESCUE, EFFECT_CAPTURED_FIGHTER_HIT, EFFECT_EXTRA_LIFE};
    trigger(THREE, 3);
    for (int32_t tick = 0; tick < 600 && sound.triggers[EFFECT_CAPTURED_FIGHTER_HIT]; tick++)
        tick_sound(&sound, 1, voices);
    CHECK_EQ(sound.triggers[EFFECT_CAPTURED_FIGHTER_HIT], 0);
    CHECK_EQ(sound.triggers[EFFECT_RESCUE], 0);
    CHECK_EQ(sound.triggers[EFFECT_EXTRA_LIFE], 1);
}

int main(void)
{
    RUN(the_effect_table_says_which_tracks_each_effect_plays);
    RUN(each_octave_down_halves_the_pitch_and_a_rest_has_none);
    RUN(the_envelope_holds_10_then_fades_one_a_tick);
    RUN(no_trigger_is_silence);
    RUN(the_shot_plays_on_the_third_voice_and_clears_its_trigger);
    RUN(a_tune_plays_to_its_end_then_clears_its_trigger);
    RUN(the_tractor_beam_trembles_between_volumes_3_and_12);
    RUN(a_later_effect_takes_the_voice_it_shares);
    RUN(the_pulse_slides_up_while_the_formation_opens_down_while_it_closes);
    RUN(shot_your_ship_clears_the_other_sounds_but_not_the_extra_fighter);
    return check_report();
}
