/* SPDX-License-Identifier: GPL-3.0-only */
#include "audio/mixer.h"
#include "test/check.h"

/** A second of sound: kept out of the stack. */
static int32_t mix[AUDIO_RATE];
static Mixer mixer;

static const Sound BEEP = {SOUND_TONE, WAVE_SQUARE, CURVE_LINEAR, CURVE_LINEAR, 1000, 1000,
                           0,          0,           100,          50,           500};
static const Sound LATE = {SOUND_TONE, WAVE_TRIANGLE, 0, 0, 440, 440, 0, 20, 10, 10, 1000};
static const Sound HISS = {SOUND_NOISE, 0, 0, 0, 0, 0, 2000, 0, 50, 0, 300};

/** Clears the first `n` samples of the mix. */
static void clear(uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        mix[i] = 0;
}

/** The loudest sample between a and b. */
static int32_t peak(uint32_t a, uint32_t b)
{
    int32_t m = 0;
    for (uint32_t i = a; i < b; i++)
        m = mix[i] > m ? mix[i] : -mix[i] > m ? -mix[i] : m;
    return m;
}

/** Sign changes between a and b: twice the frequency of a tone, over that time. */
static uint32_t crossings(uint32_t a, uint32_t b)
{
    uint32_t n = 0;
    for (uint32_t i = a + 1u; i < b; i++)
        n += (mix[i] >= 0) != (mix[i - 1u] >= 0);
    return n;
}

static void a_tone_has_its_pitch_and_volume(void)
{
    const Voice start = voice_start(&BEEP);
    Voice voice = start;
    clear(4410);
    voice_render(&voice, mix, 4410);
    CHECK_EQ(peak(0, 2000), 27525 / 2);
    CHECK(crossings(0, 2205) >= 99 && crossings(0, 2205) <= 101); /* 1000 Hz for 50 ms */
}

static void the_volume_holds_then_fades_to_silence(void)
{
    Voice voice = voice_start(&BEEP);
    clear(4410);
    voice_render(&voice, mix, 4410);
    CHECK(peak(3300, 3400) < peak(0, 2000)); /* three quarters of the way: about half */
    CHECK(peak(3300, 3400) > peak(0, 2000) / 3);
    CHECK(peak(4300, 4410) < 1500);
    CHECK(voice_is_free(&voice));
}

static void a_delayed_sound_waits(void)
{
    Voice voice = voice_start(&LATE);
    clear(1500);
    voice_render(&voice, mix, 1500);
    CHECK_EQ(peak(0, 882), 0); /* 20 ms of silence */
    CHECK(peak(882, 1323) > 30000);
}

static void a_linear_sweep_ends_at_its_pitch(void)
{
    static const Sound SWEEP = {SOUND_TONE, WAVE_SQUARE, CURVE_LINEAR, 0,   500, 2000,
                                0,          0,           200,          200, 500};
    Voice voice = voice_start(&SWEEP);
    clear(8820);
    voice_render(&voice, mix, 8820);
    CHECK(crossings(0, 441) < crossings(8379, 8820)); /* 10 ms at the start and at the end */
}

static void an_exponential_sweep_passes_the_middle_octave_halfway(void)
{
    static const Sound SWEEP = {SOUND_TONE, WAVE_SQUARE, 0, 0, 500, 2000, 0, 0, 200, 200, 500};
    Voice voice = voice_start(&SWEEP);
    clear(8820);
    voice_render(&voice, mix, 8820);
    const uint32_t middle = crossings(4410 - 441, 4410 + 441); /* 1000 Hz: 20 in 20 ms * 2 */
    CHECK(middle >= 38 && middle <= 42);
}

static void noise_is_filtered_and_quiet_at_the_end(void)
{
    Voice voice = voice_start(&HISS);
    clear(2205);
    voice_render(&voice, mix, 2205);
    CHECK(peak(0, 400) > 1000);
    CHECK(peak(2000, 2205) < peak(0, 400) / 10);
}

static void the_mixer_plays_every_layer_and_frees_its_voices(void)
{
    static const Sound LAYERS[] = {
        {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 400, 400, 0, 0, 20, 0, 200},
        {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 800, 800, 0, 10, 20, 0, 200},
    };
    static const SoundEffect EFFECT = SOUND_EFFECT(LAYERS);
    mixer = mixer_silent();
    mixer_play(&mixer, &EFFECT);
    CHECK(mixer_is_playing(&mixer));
    clear(2000);
    mixer_render(&mixer, mix, 2000);
    CHECK(!mixer_is_playing(&mixer));
}

static void a_full_mixer_replaces_the_oldest_voice(void)
{
    static const Sound ONE[] = {{SOUND_TONE, 0, 0, 0, 400, 400, 0, 0, 1000, 1000, 100}};
    static const SoundEffect EFFECT = SOUND_EFFECT(ONE);
    mixer = mixer_silent();
    for (int i = 0; i < MIXER_VOICES + 3; i++)
        mixer_play(&mixer, &EFFECT);
    CHECK_EQ(mixer.next, 3u);
    mixer_stop(&mixer);
    CHECK(!mixer_is_playing(&mixer));
}

int main(void)
{
    RUN(a_tone_has_its_pitch_and_volume);
    RUN(the_volume_holds_then_fades_to_silence);
    RUN(a_delayed_sound_waits);
    RUN(a_linear_sweep_ends_at_its_pitch);
    RUN(an_exponential_sweep_passes_the_middle_octave_halfway);
    RUN(noise_is_filtered_and_quiet_at_the_end);
    RUN(the_mixer_plays_every_layer_and_frees_its_voices);
    RUN(a_full_mixer_replaces_the_oldest_voice);
    return check_report();
}
