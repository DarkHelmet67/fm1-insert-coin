/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Where the game's screen lands on the FM-1's 240 x 240 display.
 *
 * The games are narrower than 240 pixels but not always shorter: Space Invaders is 224 x 256,
 * Galaga 224 x 288. A map says, for each display row, which game row it shows (or none), so one
 * mechanism covers every case: a screen that fits is centred, a taller one loses rows that are
 * empty in the original, and a scaled one would repeat or skip rows. Columns are never scaled:
 * the game's columns are centred, pixel for pixel.
 */
#ifndef SCREEN_MAP_H
#define SCREEN_MAP_H

#include <stdint.h>

/** The FM-1's display is SCREEN_SIZE x SCREEN_SIZE pixels. */
#define SCREEN_SIZE 240u

/** No game row: the display row stays background. */
#define SCREEN_NO_ROW (-1)

/** For each display row, the game row it shows; and where the game's columns go. */
typedef struct {
    int16_t source_row[SCREEN_SIZE]; /**< a game row, or SCREEN_NO_ROW */
    uint16_t source_x;               /**< the first game column shown */
    uint16_t dest_x;                 /**< the display column it lands on */
    uint16_t width;                  /**< how many columns are shown */
} ScreenMap;

/** A run of game rows, first to last included. */
typedef struct {
    uint16_t first;
    uint16_t last;
} RowRange;

/**
 * The game's screen at 1:1, centred; a side larger than the display is cropped evenly.
 * @param width  the game's width in pixels
 * @param height the game's height in pixels
 */
ScreenMap screen_map_centered(uint32_t width, uint32_t height);

/**
 * Only some runs of game rows, stacked one under the other and centred: the rows left out are
 * the ones not worth a display row (an empty margin, a gap under a line).
 * @param width  the game's width in pixels (centred, cropped evenly if wider than the display)
 * @param ranges the runs of rows, top to bottom; rows past the display's bottom are dropped
 * @param count  how many runs
 */
ScreenMap screen_map_rows(uint32_t width, const RowRange *ranges, uint32_t count);

#endif
