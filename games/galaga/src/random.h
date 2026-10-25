/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * A pure source of random bytes (random.ts): the n-th byte for a given seed, from an integer
 * hash (the "lowbias32" mix by Chris Wellons). The game takes the seed from the frame counter
 * when a stage starts, so the same controls always give the same game.
 */
#ifndef GALAGA_RANDOM_H
#define GALAGA_RANDOM_H

#include <stdint.h>

#include "cartridge/private.h"

/** Random byte `n` of `seed`. */
GAME_PRIVATE int32_t random_byte(int32_t seed, int32_t n);

#endif
