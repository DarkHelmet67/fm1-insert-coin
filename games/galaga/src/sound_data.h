/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Reading the data of the sound program (sound-data.ts, `gg1-7.s`), as the sound processor
 * does: the note table, the effect table, the tracks and their tempos. Addresses are those of the
 * ROM used (3700g.bin); the labels of the source, written on another set, are $27 lower.
 */
#ifndef GALAGA_SOUND_DATA_H
#define GALAGA_SOUND_DATA_H

#include <stdint.h>

#include "cartridge/private.h"

/** `d_06C3`: the 8 steps and 8 start pitches of the pulsing sound, opening and closing. */
#define PULSE_OPENING 0x06ea
#define PULSE_CLOSING 0x06fa

/** A note byte that silences the voice, and the one that ends a track. */
#define NOTE_REST 0x0c
#define NOTE_END 0xff

/** The tracks an effect plays, one per voice. */
typedef struct {
    int32_t first;
    int32_t count;
    int32_t voice; /**< the voice of the first track; the next tracks take the next voices */
} EffectTracks;

/** The 3 bytes that open a track, before its notes. */
typedef struct {
    int32_t envelope; /**< 0 steady then fading, 1 rising, 2 or more falling */
    int32_t hold;     /**< ticks at full volume before the fade; 0 holds the note */
    int32_t waveform;
    int32_t notes; /**< address of the notes: pairs of a note byte and a length in beats */
} TrackHeader;

/** One part of the pulsing sound: a sliding pitch. */
typedef struct {
    int32_t step;
    int32_t start;
} PulseSegment;

/** A byte of the sound ROM; outside the data, the ROM is blank ($FF). */
GAME_PRIVATE int32_t sound_byte(int32_t address);

/** The tracks of `effect`, as the effect table lists them. */
GAME_PRIVATE EffectTracks effect_tracks(int32_t effect);

/** The header of `track`. */
GAME_PRIVATE TrackHeader track_header(int32_t track);

/** The beat of `effect`, in ticks. */
GAME_PRIVATE int32_t tempo(int32_t effect);

/**
 * `l_05A7`: the 16-bit pitch of a note byte. The low nibble is the note in the top octave, the
 * high nibble how many octaves lower: each halves the pitch.
 */
GAME_PRIVATE int32_t note_pitch(int32_t note);

/** Segment `segment` of the pulsing table at `table`: the start pitches follow the steps. */
GAME_PRIVATE PulseSegment pulse_segment(int32_t table, int32_t segment);

#endif
