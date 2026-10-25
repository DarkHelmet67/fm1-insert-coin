/* SPDX-License-Identifier: GPL-3.0-only */
#include "sounds.h"

#include "config.h"

/*
 * Fields: kind, wave, sweep, fade, from, to, cutoff, delay, duration, hold, volume (sound.h).
 * A beep holds its volume to the end, then stops: a plain square wave.
 */
#define BEEP(hz, ms, delay, hold)                                                                  \
    {                                                                                              \
        SOUND_TONE, WAVE_SQUARE, 0, CURVE_LINEAR, hz, hz, 0, delay, ms, hold, BEEP_VOLUME          \
    }

static const Sound PADDLE[] = {BEEP(PADDLE_BEEP_HZ, PADDLE_BEEP_MS, 0, PADDLE_BEEP_MS)};
static const Sound WALL[] = {BEEP(WALL_BEEP_HZ, WALL_BEEP_MS, 0, WALL_BEEP_MS)};
static const Sound BRICK[] = {BEEP(BRICK_BEEP_HZ, BRICK_BEEP_MS, 0, BRICK_BEEP_MS)};

/**
 * The record beaten during play (not in the original, which kept no record): a major arpeggio
 * climbing to the paddle's pitch, each note held for 70% of its length, the last one longer.
 */
static const Sound NEW_RECORD[] = {
    BEEP(500, 120, 0, 84),
    BEEP(630, 120, 140, 84),
    BEEP(750, 120, 280, 84),
    BEEP(1000, 600, 420, 420),
};

GAME_PRIVATE const SoundEffect SOUND_EFFECTS[SOUND_COUNT] = {
    [SOUND_PADDLE] = SOUND_EFFECT(PADDLE),
    [SOUND_WALL] = SOUND_EFFECT(WALL),
    [SOUND_BRICK] = SOUND_EFFECT(BRICK),
    [SOUND_NEW_RECORD] = SOUND_EFFECT(NEW_RECORD),
};

/** The ball turned back up on the paddle row. */
static int hit_paddle(const Ball *prev, const Ball *next)
{
    return prev->dir_y == 1 && next->dir_y == -1 &&
           next->y_fifths == (PADDLE_Y - BALL_HEIGHT) * FIFTHS;
}

/** The ball turned sideways against a side wall. */
static int hit_side_wall(const Ball *prev, const Ball *next)
{
    return prev->dir_x != next->dir_x && (next->x_fifths == SIDE_WALL_WIDTH * FIFTHS ||
                                          next->x_fifths == (RIGHT_WALL_X - BALL_WIDTH) * FIFTHS);
}

/** The ball turned down against the top wall. */
static int hit_top_wall(const Ball *prev, const Ball *next)
{
    return TOP_WALL_SOUND && prev->dir_y == -1 && next->dir_y == 1 &&
           next->y_fifths == TOP_WALL_HEIGHT * FIFTHS;
}

/** The sounds of the ball in this frame: the paddle, or a wall. */
static uint32_t ball_sounds(const Play *prev, const Play *next)
{
    if (prev->phase != PHASE_IN_PLAY || next->phase != PHASE_IN_PLAY)
        return 0;
    if (hit_paddle(&prev->ball, &next->ball))
        return SOUND_BIT(SOUND_PADDLE);
    if (hit_side_wall(&prev->ball, &next->ball) || hit_top_wall(&prev->ball, &next->ball))
        return SOUND_BIT(SOUND_WALL);
    return 0;
}

/**
 * Whether this frame beat the record held when the game started: once per game, and only if
 * there was a record (the first brick of the first game ever is not a celebration).
 */
static int beat_record(const GameState *previous, const GameState *next)
{
    return previous->record_to_beat > 0 && previous->round.score <= previous->record_to_beat &&
           next->round.score > previous->record_to_beat;
}

uint32_t sounds_for(const GameState *previous, const GameState *next)
{
    if (previous->attract || next->attract)
        return 0;
    uint32_t mask = ball_sounds(&previous->round.play, &next->round.play);
    if (next->ticks.played > previous->ticks.played)
        mask |= SOUND_BIT(SOUND_BRICK);
    if (beat_record(previous, next))
        mask |= SOUND_BIT(SOUND_NEW_RECORD);
    return mask;
}
