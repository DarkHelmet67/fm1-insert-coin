/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Reading the ROM tables safely. The web version reads `table[i] ?? 0`: an index outside the
 * table gives 0 instead of whatever lies beyond it, and the program sometimes does read past the
 * end of a table (bomber-timers.ts says why). The C port does the same.
 */
#ifndef GALAGA_TABLE_H
#define GALAGA_TABLE_H

#include <stdint.h>

#include "cartridge/private.h"

/** Byte `index` of `table` (of `size` bytes), or `fallback` outside it. */
GAME_PRIVATE int32_t table_at(const uint8_t *table, uint32_t size, int32_t index, int32_t fallback);

/** Byte `index` of a table of rom.h, 0 outside it: TABLE(SCORE_STEPS, i). */
#define TABLE(name, index) table_at((name), name##_SIZE, (index), 0)

#endif
