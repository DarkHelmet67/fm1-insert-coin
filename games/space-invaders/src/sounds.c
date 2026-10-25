/* SPDX-License-Identifier: GPL-3.0-only */
#include "sounds.h"

/*
 * The numbers of sound-bank.ts, in hertz, milliseconds and thousandths of volume. Fields:
 * kind, wave, sweep, fade, from, to, cutoff, delay, duration, hold, volume. Zero is the web
 * version's default (exponential sweep and fade, no filter, no delay, no hold).
 */

/** A short rising chirp when a coin is accepted (the original has no coin sound). */
static const Sound COIN[] = {
    {SOUND_TONE, WAVE_SQUARE, 0, 0, 660, 1320, 0, 0, 150, 0, 150},
};

/** The laser: a hiss, then a pitch falling in straight lines, again and again. */
static const Sound SHOT[] = {
    {SOUND_NOISE, 0, 0, 0, 0, 0, 4000, 0, 50, 0, 60},
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, 0, 800, 375, 0, 0, 58, 58, 220},
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, CURVE_LINEAR, 1800, 450, 0, 58, 200, 50, 200},
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, CURVE_LINEAR, 1790, 1330, 0, 248, 85, 0, 70},
};

/** An invader hit: a high tone around 2.6 kHz that sags slightly and fades after 0.16 s. */
static const Sound ALIEN_HIT[] = {
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, CURVE_LINEAR, 2660, 2420, 0, 0, 330, 160, 150},
};

/** One chirp of the mystery ship's warble: up from 780 Hz, then down to 820 Hz. */
static const Sound UFO[] = {
    {SOUND_TONE, WAVE_TRIANGLE, 0, 0, 780, 3100, 0, 0, 80, 80, 80},
    {SOUND_TONE, WAVE_TRIANGLE, 0, 0, 2000, 820, 0, 80, 83, 83, 80},
};

/** The mystery ship destroyed: a long falling tone (no recording available: chosen by ear). */
static const Sound UFO_HIT[] = {
    {SOUND_TONE, WAVE_SQUARE, 0, 0, 1600, 200, 0, 0, 800, 0, 150},
};

/** The cannon destroyed: 0.8 s of low noise, steady for 0.55 s, then fading. */
static const Sound CANNON_HIT[] = {
    {SOUND_NOISE, 0, 0, CURVE_LINEAR, 0, 0, 700, 0, 800, 550, 450},
};

/** A bonus cannon: the circuit analysis gives a 480 Hz tone; the length is chosen by ear. */
static const Sound EXTRA_LIFE[] = {
    {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 480, 480, 0, 0, 1000, 800, 100},
};

/** The hi-score beaten during play (not in the original): the bonus tone climbing an arpeggio. */
static const Sound NEW_RECORD[] = {
    {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 480, 480, 0, 0, 120, 84, 100},
    {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 600, 600, 0, 140, 120, 84, 100},
    {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 720, 720, 0, 280, 120, 84, 100},
    {SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, 960, 960, 0, 420, 600, 420, 100},
};

/** One note of the march: a low sawtooth softened by a filter, held steady and cut short. */
/* (clang-format 18 and 21 lay out a braced macro differently: kept as it is) */
/* clang-format off */
#define MARCH_NOTE(hz) {SOUND_TONE, WAVE_SAWTOOTH, 0, CURVE_LINEAR, hz, hz, 500, 0, 100, 75, 350}
/* clang-format on */
static const Sound MARCH_0[] = {MARCH_NOTE(60)};
static const Sound MARCH_1[] = {MARCH_NOTE(56)};
static const Sound MARCH_2[] = {MARCH_NOTE(52)};
static const Sound MARCH_3[] = {MARCH_NOTE(69)};

GAME_PRIVATE const SoundEffect SOUND_EFFECTS[SOUND_COUNT] = {
    [SOUND_COIN] = SOUND_EFFECT(COIN),
    [SOUND_SHOT] = SOUND_EFFECT(SHOT),
    [SOUND_ALIEN_HIT] = SOUND_EFFECT(ALIEN_HIT),
    [SOUND_UFO] = SOUND_EFFECT(UFO),
    [SOUND_UFO_HIT] = SOUND_EFFECT(UFO_HIT),
    [SOUND_CANNON_HIT] = SOUND_EFFECT(CANNON_HIT),
    [SOUND_EXTRA_LIFE] = SOUND_EFFECT(EXTRA_LIFE),
    [SOUND_NEW_RECORD] = SOUND_EFFECT(NEW_RECORD),
    [SOUND_MARCH_0] = SOUND_EFFECT(MARCH_0),
    [SOUND_MARCH_1] = SOUND_EFFECT(MARCH_1),
    [SOUND_MARCH_2] = SOUND_EFFECT(MARCH_2),
    [SOUND_MARCH_3] = SOUND_EFFECT(MARCH_3),
};

/** Frames between two chirps of the mystery ship's warble (one chirp: 0.163 s). */
#define UFO_WARBLE_FRAMES 10u

/** Whether an effect of `kind` appeared in this frame. */
static int effect_started(const PlayingState *state, EffectKind kind)
{
    for (uint32_t i = 0; i < state->effects.count; i++)
        if (state->effects.list[i].kind == kind &&
            state->effects.list[i].frames_left == effect_frames(kind))
            return 1;
    return 0;
}

/** The sounds of a frame of play, from the change between two states. */
static uint32_t playing_sounds(const PlayingState *prev, const PlayingState *next)
{
    uint32_t mask = 0;
    if (next->fleet.beat != prev->fleet.beat)
        mask |= SOUND_BIT(SOUND_MARCH_0 + next->fleet.beat % 4u);
    if (next->shots_fired > prev->shots_fired)
        mask |= SOUND_BIT(SOUND_SHOT);
    if (effect_started(next, EFFECT_ALIEN))
        mask |= SOUND_BIT(SOUND_ALIEN_HIT);
    if (effect_started(next, EFFECT_UFO))
        mask |= SOUND_BIT(SOUND_UFO_HIT);
    if (!prev->cannon_explosion && next->cannon_explosion)
        mask |= SOUND_BIT(SOUND_CANNON_HIT);
    if (!prev->extra_life_awarded && next->extra_life_awarded)
        mask |= SOUND_BIT(SOUND_EXTRA_LIFE);
    if (next->ufo.flying && next->ufo.age % UFO_WARBLE_FRAMES == 0u)
        mask |= SOUND_BIT(SOUND_UFO);
    return mask;
}

/**
 * Whether this frame beat the record held when the game started: once per game, and only if
 * there was a record (the first invader of the first game ever is not a celebration).
 */
static int beat_record(const GameState *previous, const PlayingState *next)
{
    return previous->record_to_beat > 0 && previous->playing.score <= previous->record_to_beat &&
           next->score > previous->record_to_beat;
}

uint32_t sounds_for(const GameState *previous, const GameState *next)
{
    if (previous->screen != SCREEN_PLAYING && next->screen == SCREEN_PLAYING)
        return SOUND_BIT(SOUND_COIN);
    if (previous->screen != SCREEN_PLAYING || next->screen == SCREEN_ATTRACT)
        return 0;
    uint32_t mask = playing_sounds(&previous->playing, &next->playing);
    if (beat_record(previous, &next->playing))
        mask |= SOUND_BIT(SOUND_NEW_RECORD);
    return mask;
}
