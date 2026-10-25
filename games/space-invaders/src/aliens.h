/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The invaders of the formation (insert-coin, aliens.ts). */
#ifndef ALIENS_H
#define ALIENS_H

#include <stdint.h>

#include "sprites.h"

/** Rows and columns of the formation: 55 invaders. */
#define FORMATION_ROWS 5
#define FORMATION_COLUMNS 11
#define FORMATION_SIZE (FORMATION_ROWS * FORMATION_COLUMNS)

/** Size of the grid cell holding each invader, and left edge of the first column. */
#define CELL_SIZE 16
#define FORMATION_LEFT 24

/** Height of the bottom row in the first round (0x78 in the original's rotated coordinates). */
#define FIRST_ROUND_BOTTOM_Y 128

/** Height of an invader, of every kind. */
#define ALIEN_HEIGHT 8

/** One invader of the formation. */
typedef struct {
    uint8_t kind;   /**< an AlienKind */
    uint8_t row;    /**< 1 (top) to 5 (bottom): decides the colour */
    uint8_t column; /**< 1 (left) to 11 (right): used by the bomb firing tables */
    uint8_t frame;  /**< animation frame, 0 or 1: it switches at every step */
    int16_t x;      /**< top-left corner of the sprite, in screen pixels */
    int16_t y;
} Alien;

/** Width in pixels of an invader's sprite. */
int32_t alien_width(const Alien *alien);

/** Points scored for destroying an invader. */
int32_t alien_points(const Alien *alien);

/**
 * Height of the bottom row at the start of `round` (1-based). From round 10 the original's
 * table starts over at its first entry: the first round's height is never used again.
 */
int32_t start_bottom_y(uint32_t round);

/**
 * The invader at `index` of the formation at the start of `round`. The formation is in
 * marching order, as in the original: bottom row first, left to right.
 */
Alien formation_alien(uint32_t index, uint32_t round);

#endif
