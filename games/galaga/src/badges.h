/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `c_new_level_tokens`: the stage badges in the bottom right corner (badges.ts). Stages are
 * counted with flags worth 50, 30, 20, 10, 5 and 1, like coins; each flag is one or two columns
 * of two characters, drawn from the left, the last one in the rightmost column.
 */
#ifndef GALAGA_BADGES_H
#define GALAGA_BADGES_H

#include <stdint.h>

#include "cartridge/private.h"

/** One column of a badge: the characters on rows 34 and 35, and their palette. */
typedef struct {
    uint8_t top;
    uint8_t bottom;
    uint8_t palette;
} BadgeColumn;

/** The most flags a stage of a game needs (stage 99: 50, 30, 10, 5 and four 1s). */
#define MAX_FLAGS 16u

/** The column of the rightmost badge column: the flags end at the right edge. */
#define BADGES_RIGHT 27

/** How many flags `stage` has: one per "click" when they appear. */
GAME_PRIVATE uint32_t stage_flag_count(int32_t stage);

/**
 * The columns of the first `shown` flags of `stage`, from the leftmost, into `out` (room for
 * 2 * MAX_FLAGS); returns how many.
 */
GAME_PRIVATE uint32_t badge_columns(int32_t stage, uint32_t shown, BadgeColumn *out);

#endif
