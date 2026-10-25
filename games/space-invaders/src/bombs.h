/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The invaders' bombs: at most one of each kind on screen (insert-coin, bombs.ts). */
#ifndef BOMBS_H
#define BOMBS_H

#include <stdint.h>

#include "fleet.h"

/** A bomb dropped by an invader. */
typedef struct {
    uint8_t kind;  /**< a BombKind */
    uint8_t steps; /**< steps since it was dropped: drives the animation and the reload rule */
    int16_t x;     /**< top-left corner of the 3x8 sprite */
    int16_t y;
} Bomb;

/** The bombs on screen and the firing tables' positions. */
typedef struct {
    Bomb active[BOMB_KIND_COUNT]; /**< in the order they were dropped */
    uint8_t count;
    uint8_t plunger_index; /**< position in the column table of the plunger bombs */
    uint8_t squiggly_index;
    uint32_t turn; /**< frame counter: each frame handles one kind, in turn */
} Bombs;

/** What the bombs need to know about the rest of the game in this frame. */
typedef struct {
    const Fleet *fleet;
    int32_t cannon_center_x;
    int32_t score;
} BombContext;

/** No bombs on screen. */
Bombs bombs_none(void);

/** Pixels a bomb falls at each of its steps: 4, or 5 when 8 invaders or fewer are left. */
int32_t bomb_speed(uint32_t aliens_left);

/** How many steps the falling bombs must have taken before a new one can drop. */
uint32_t reload_steps(int32_t score);

/** Index in the fleet of the lowest invader of `column`, or -1 when the column is empty. */
int32_t lowest_in_column(const Fleet *fleet, uint32_t column);

/** The column right above `x`, or 0 when no invader is there: the rolling bomb aims there. */
uint32_t column_above(const Fleet *fleet, int32_t x);

/** Advances the bombs by one frame: the kind whose turn it is either falls or tries to drop. */
Bombs bombs_step(const Bombs *bombs, const BombContext *context);

/** Removes the bomb at `index`, keeping the order of the others. */
Bombs bombs_remove(const Bombs *bombs, uint32_t index);

#endif
