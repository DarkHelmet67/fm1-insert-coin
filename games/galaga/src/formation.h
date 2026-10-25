/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The formation grid (formation.ts): 10 columns and 6 rows (`db_fmtn_hpos_orig`). Each column
 * and each row has an origin and an offset that the sway and the "breathing" of the formation
 * change; the sprites of the resting aliens follow the sum.
 */
#ifndef GALAGA_FORMATION_H
#define GALAGA_FORMATION_H

#include <stdint.h>

#include "cartridge/private.h"

/** Columns are cells 0-9 (x), rows cells 10-15 (y). */
#define COLUMNS 10
#define ROWS 6
#define CELLS (COLUMNS + ROWS)

/** The grid as it is at one moment. */
typedef struct {
    uint8_t offsets[CELLS];        /**< offset of each cell from its origin (`ds_hpos_loc_offs`) */
    uint16_t sprite_coords[CELLS]; /**< x of columns, 9-bit y of rows (`ds_hpos_spcoords`) */
    /**
     * `ds_9200_glbls[$0F]`: while swaying, 0 moving right and 1 moving left; while breathing, a
     * counter that goes up from 0 to $1F (opening) and down from $A0 to $81 (closing).
     */
    uint8_t direction;
    uint8_t bitmap[CELLS]; /**< the working copy of the breathing bitmap: which cells move */
} Formation;

/** What the sway did in a frame. */
typedef struct {
    Formation formation;
    int settled; /**< the grid is back in the middle after the last wave: breathing starts now */
} SwayResult;

/** The offset of `cell`; 0 for a number that is not a cell, as the web version's `?? 0`. */
GAME_PRIVATE int32_t formation_offset(const Formation *formation, int32_t cell);

/** The sprite coordinate of `cell`; 0 for a number that is not a cell. */
GAME_PRIVATE int32_t formation_coord(const Formation *formation, int32_t cell);

/** `c_12C3`: the grid at its origin, as each stage starts. */
GAME_PRIVATE Formation initial_formation(void);

/**
 * Origin of a cell (`ds_hpos_loc_orig`): column x in pixels, row height in the flight
 * coordinates of the second processor (units of 2 pixels, growing upwards).
 */
GAME_PRIVATE int32_t cell_origin(int32_t cell);

/**
 * `f_2A90`: while the waves enter, the grid sways left and right by 32 pixels, one pixel every
 * 4 frames. Once all aliens are home (`stop_in_middle`), it stops when back in the middle.
 */
GAME_PRIVATE SwayResult sway_formation(const Formation *formation, int32_t frame,
                                       int stop_in_middle);

/**
 * `f_1DE6`: once the attacks start, the grid "breathes": every 4 frames some cells move one
 * pixel outwards (or inwards). The outer columns and the lower rows move more often, chosen by
 * one of four bitmaps that changes every 32 frames, in time with the flapping of the wings.
 */
GAME_PRIVATE Formation breathe_formation(const Formation *formation, int32_t frame);

#endif
