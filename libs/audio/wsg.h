/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Namco's WSG ("waveform sound generator"), the sound chip of Galaga and Pac-Man: three voices,
 * each playing one of 8 waveforms of 32 four-bit samples from a PROM, at a 20-bit frequency and
 * a volume from 0 to 15. The game's sound program sets the voices 120 times a second (two
 * "ticks" a frame); the chip plays them. Rendering changes the chip, like drawing changes a
 * framebuffer: it runs in the sound interrupt, 44,100 times a second.
 */
#ifndef WSG_H
#define WSG_H

#include <stdint.h>

/** The chip has three voices. */
#define WSG_VOICES 3

/** The sound program ticks twice a frame (at lines 64 and 192 of the screen, MAME). */
#define WSG_TICKS 2

/** Samples of one 60 Hz frame (44,100 / 60): the first tick plays for half of them, the second
 * after. */
#define WSG_FRAME_SAMPLES 735u

/** Samples in one waveform, and bytes of the waveform PROM (8 waveforms). */
#define WSG_WAVE_LENGTH 32
#define WSG_PROM_SIZE 256

/** What one voice plays. */
typedef struct {
    uint32_t frequency; /**< 20-bit step added to the voice's counter at every chip sample */
    uint8_t volume;     /**< 0 (silent) to 15 */
    uint8_t waveform;   /**< which waveform of the PROM, 0 to 7 */
} WsgVoice;

/** The voices of the two ticks of a frame. */
typedef struct {
    WsgVoice tick[WSG_TICKS][WSG_VOICES];
} WsgFrame;

/** The chip between two calls: where each voice is in its waveform. */
typedef struct {
    uint32_t counters[WSG_VOICES]; /**< 20-bit counters with 4 more bits of fraction */
    uint32_t position; /**< samples played of the current frame, up to WSG_FRAME_SAMPLES */
} Wsg;

/** A frame of silence. */
WsgFrame wsg_silent_frame(void);

/** The chip at power-on. */
Wsg wsg_new(void);

/**
 * The counter step of a voice for one sample at 44.1 kHz, in 1/16 of the chip's units. The chip
 * makes 96,000 samples a second (the 18.432 MHz clock divided by 6 and by 32): 96000 / 44100 is
 * 2229 / 1024 within 0.01%.
 */
uint32_t wsg_step(uint32_t frequency);

/** Sample `index` (0-31) of waveform `waveform` of the PROM, from -8 to 7. */
int32_t wsg_sample(const uint8_t prom[WSG_PROM_SIZE], int32_t waveform, int32_t index);

/**
 * Adds `n` samples of `frame` to `mix` (Q15): the first tick for the first half of the frame,
 * the second after it, which holds past the frame's end until wsg_start_frame() is called again.
 */
void wsg_render(Wsg *wsg, const WsgFrame *frame, const uint8_t prom[WSG_PROM_SIZE], int32_t *mix,
                uint32_t n);

/** Samples left before the current frame ends: 0 once it has played whole (the second tick holds).
 */
uint32_t wsg_frame_left(const Wsg *wsg);

/** A new frame of voices begins: the first tick plays again. */
void wsg_start_frame(Wsg *wsg);

#endif
