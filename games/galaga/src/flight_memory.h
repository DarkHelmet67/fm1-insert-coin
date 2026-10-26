/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The data areas of the second processor's ROM (flight-memory.ts), addressed like the original:
 * flight programs jump to each other by address.
 */
#ifndef GALAGA_FLIGHT_MEMORY_H
#define GALAGA_FLIGHT_MEMORY_H

#include <stdint.h>

#include "cartridge/private.h"

/** Where an object rests in the formation: the indexes of its row (10-15) and column (0-9). */
typedef struct {
    int32_t row;
    int32_t column;
} HomeCell;

/**
 * The byte at `address`. Reading outside the data areas would be a bug in a flight program: it
 * reads $FF, the "stop flying" command, so the alien just disappears.
 */
GAME_PRIVATE int32_t rom_byte(int32_t address);

/** The little-endian 16-bit word at `address`. */
GAME_PRIVATE int32_t rom_word(int32_t address);

/** Where object `id` rests in the formation (`sprt_fmtn_hpos`). */
GAME_PRIVATE HomeCell home_cell(int32_t id);

#endif
