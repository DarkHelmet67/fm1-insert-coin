/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Sounds described as plain data, as in insert-coin's @arcade/audio (sound.ts): a tone that
 * slides from one pitch to another, or a burst of filtered noise, with a volume that holds and
 * then fades. The same numbers as the web version, in whole units (hertz, milliseconds, volume
 * in thousandths). A field left at zero takes the web version's default.
 */
#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

/** A tone or a noise burst. */
typedef enum { SOUND_TONE, SOUND_NOISE } SoundKind;

/** The oscillator's wave. */
typedef enum { WAVE_SQUARE, WAVE_TRIANGLE, WAVE_SAWTOOTH } Wave;

/** How a pitch slides, or a volume falls: even to the ear (the default), or in a straight line. */
typedef enum { CURVE_EXPONENTIAL, CURVE_LINEAR } Curve;

/** One sound. */
typedef struct {
    uint8_t kind;         /**< a SoundKind */
    uint8_t wave;         /**< a Wave, for tones */
    uint8_t sweep;        /**< a Curve: how the pitch slides from `from_hz` to `to_hz` */
    uint8_t fade;         /**< a Curve: how the volume falls after the hold */
    uint16_t from_hz;     /**< starting pitch of a tone */
    uint16_t to_hz;       /**< ending pitch of a tone */
    uint16_t cutoff_hz;   /**< low-pass filter; 0 = none (a noise always has one) */
    uint16_t delay_ms;    /**< wait before starting, to line up the layers of an effect */
    uint16_t duration_ms; /**< from the start to the end of the fade */
    uint16_t hold_ms;     /**< at full volume before the fade starts */
    uint16_t volume;      /**< starting volume, in thousandths (1000 = full scale) */
} Sound;

/** A sound effect: one or more sounds played together, each after its own delay. */
typedef struct {
    const Sound *layers;
    uint8_t count;
} SoundEffect;

/** An effect made of the sounds of an array. */
#define SOUND_EFFECT(array)                                                                        \
    {                                                                                              \
        (array), (uint8_t)(sizeof(array) / sizeof((array)[0]))                                     \
    }

#endif
