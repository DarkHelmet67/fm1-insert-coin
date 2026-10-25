/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Powers and logarithms of two in fixed point, with integers only: the chip has no FPU. Pitch
 * sweeps and volume fades that sound "even" to the ear are straight lines in octaves (log2), so
 * the synthesizer works in octaves and turns them back into numbers with these two functions.
 */
#ifndef POW2_H
#define POW2_H

#include <stdint.h>

/** One octave in the Q16 fixed point of these functions. */
#define OCTAVE_Q16 65536

/** log2(x) in Q16 (65536 = one octave), for x >= 1; log2(0) is taken as 0. */
int32_t log2_q16(uint32_t x);

/**
 * value × 2^(exponent), with the exponent in Q16. Accurate to about 0.1 %.
 * @param value below 2^17, so the intermediate product fits 32 bits
 * @param exponent_q16 positive to multiply, negative to divide
 */
uint32_t scale_pow2(uint32_t value, int32_t exponent_q16);

#endif
