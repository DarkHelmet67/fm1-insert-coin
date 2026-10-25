/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The arcade games run at 60 frames a second, the FM-1 counts milliseconds: how many frames a
 * number of milliseconds is worth, with no drift (60 frames in every 1000 ms) and no 64-bit
 * division, which the chip cannot do.
 */
#ifndef FRAME_CLOCK_H
#define FRAME_CLOCK_H

#include <stdint.h>

/** Frames a second, as the original monitors. */
#define FRAME_RATE 60u

/**
 * The frames due in `ms` milliseconds: a frame every 16.67 ms, as whole frames.
 * @param ms milliseconds since the clock started
 * @returns whole frames; the main loop runs the game until it has run this many
 */
uint32_t frame_clock_frames(uint32_t ms);

#endif
