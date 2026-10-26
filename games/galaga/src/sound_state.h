/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The memory of the sound processor (sound-state.ts, `b_9A00`-`b_9AD6` in `gg1-7.s`): the
 * trigger registers the main processor writes, which effects are playing, the tracks and the few
 * counters of the effects that do more than play notes; and the voices being set up in a tick.
 */
#ifndef GALAGA_SOUND_STATE_H
#define GALAGA_SOUND_STATE_H

#include <stdint.h>

#include "cartridge/private.h"

/** Effects in the program: $00 to $16. */
#define EFFECT_COUNT 0x17

/** Tracks in the program: $00 to $2E. */
#define TRACK_COUNT 0x2f

/** The voices of the sound chip. */
#define SOUND_VOICES 3

/** The effects by name (`b_9AA0[n]`). */
enum {
    EFFECT_PULSE = 0x00,
    EFFECT_BOSS_HIT = 0x01,
    EFFECT_BUTTERFLY_HIT = 0x02,
    EFFECT_BEE_HIT = 0x03,
    EFFECT_BOSS_DAMAGED = 0x04,
    EFFECT_TRACTOR_BEAM = 0x05,
    EFFECT_BEAM_CAPTURE = 0x06,
    EFFECT_CAPTURED_FIGHTER_HIT = 0x07,
    EFFECT_COIN = 0x08,
    EFFECT_FIGHTER_CAPTURED = 0x09,
    EFFECT_EXTRA_LIFE = 0x0a,
    EFFECT_START_THEME = 0x0b,
    EFFECT_NAME_ENTRY = 0x0c,
    EFFECT_CHALLENGE_START = 0x0d,
    EFFECT_CHALLENGE_END = 0x0e,
    EFFECT_SHOT = 0x0f,
    EFFECT_UNUSED = 0x10,
    EFFECT_RESCUE = 0x11,
    EFFECT_BONUS_BEE = 0x12,
    EFFECT_DIVE = 0x13,
    EFFECT_PERFECT = 0x14,
    EFFECT_BADGE = 0x15,
    EFFECT_NAME_ENTRY_END = 0x16,
};

/** The pulsing sound of the breathing formation (`b_9A80`-`b_9A87`). */
typedef struct {
    uint8_t sign;    /**< the formation's direction it was started for: 1 opening, $FF closing */
    uint16_t table;  /**< address of the table in use */
    uint8_t segment; /**< the segment of it */
    uint16_t pitch;  /**< the pitch, 16 bits */
    uint16_t step;   /**< how much it changes at every tick */
} Pulse;

/** What the sound processor keeps about its tracks (`b_9A00`, `b_9A30`). */
typedef struct {
    uint8_t ticks[TRACK_COUNT]; /**< ticks played of the current note of each track */
    uint8_t notes[TRACK_COUNT]; /**< where each track is in its notes, in bytes (2 per note) */
} Tracks;

/** The sound processor's memory. */
typedef struct {
    uint8_t triggers[EFFECT_COUNT]; /**< the trigger registers of the main processor (`b_9AA0`) */
    uint8_t playing[EFFECT_COUNT];  /**< which effects are playing (`b_9AC0`) */
    Tracks tracks;
    uint8_t waveforms[SOUND_VOICES]; /**< the waveform of each voice: it stays between ticks */
    Pulse pulse;
    uint8_t beam_volume;   /**< volume of the third voice's tremolo in the beam (`b_9A7C`) */
    uint8_t beam_ticks;    /**< its tick counter (`b_9A7E`) */
    uint8_t pull_waveform; /**< waveform of the fighter pulled up the beam (`b_9A7D`) */
    uint8_t pull_ticks;    /**< its tick counter (`b_9A7F`) */
} SoundState;

/** What the effects ask of the three voices in one tick (`b_9A60`, `b_9A70`). */
typedef struct {
    int32_t frequencies[SOUND_VOICES]; /**< 20-bit frequency of each voice; cleared every tick */
    int32_t volumes[SOUND_VOICES];     /**< volume, in the low 4 bits; cleared every tick */
    int32_t waveforms[SOUND_VOICES];   /**< waveform: it stays until an effect changes it */
} Mix;

/**
 * What one tick works on: the memory (the board's working copy, see flight.h) and the voices
 * being set up.
 */
typedef struct {
    SoundState *sound;
    Mix mix;
} Work;

/** The memory cleared at power-on (`l_00A2`). */
GAME_PRIVATE SoundState new_sound_state(void);

#endif
