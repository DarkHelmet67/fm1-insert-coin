/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The arithmetic routines of the second processor, ported bit for bit (z80-math.ts). Flight
 * paths depend on their rounding.
 */
#ifndef GALAGA_Z80_MATH_H
#define GALAGA_Z80_MATH_H

#include <stdint.h>

#include "cartridge/private.h"

/**
 * `c_0EAA`: 16-bit by 8-bit division, the Z80 way (shift and subtract, 17 rounds). Returns the
 * 16-bit quotient; dividing by 0 gives $FFFF, as on the hardware.
 */
GAME_PRIVATE int32_t z80_divide(int32_t dividend, int32_t divisor);

/** A byte read as a signed number (-128 to 127). */
GAME_PRIVATE int32_t signed8(int32_t byte);

/**
 * `c_0E5B`: the direction from (from_y, from_x) to (to_y, to_x), in units of 2 pixels, as a
 * 10-bit angle (0 right, $100 up). The octant comes from the signs and from which difference is
 * larger; inside the octant, the ratio of the smaller difference to the larger is a good enough
 * arctangent.
 */
GAME_PRIVATE int32_t angle_to(int32_t to_y, int32_t to_x, int32_t from_y, int32_t from_x);

#endif
