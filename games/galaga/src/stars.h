/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The starfield of the Namco 05xx chip (stars.ts), as described and emulated by MAME
 * (starfield_05xx.cpp, R. Hildinger 2019). The chip runs a 16-bit shift register one step per
 * pixel over a 256 x 256 raster and lights a star whenever 8 of its bits match a fixed value:
 * 256 possible stars, in 4 sets of 64. Because the register repeats every 65,535 steps but a
 * frame lasts 65,536, the stars drift by one pixel each frame; running a few steps more or fewer
 * per frame makes them scroll faster, slower or stand still.
 *
 * The 256 stars were found once, by tools/galaga/extract-rom.mjs (STAR_SEEDS in rom.h): each
 * frame only computes where each one falls.
 */
#ifndef GALAGA_STARS_H
#define GALAGA_STARS_H

#include <stdint.h>

#include "cartridge/private.h"

/**
 * The 8 scroll speeds of the chip: 7 stands still, 6, 5 and 4 move the stars down 1, 2 and 3
 * pixels a frame, 0-3 move them up 1-4 pixels. Galaga only uses the downward ones.
 */
#define STARS_STILL 7
#define STARS_SLOW 6

/** The scrolling starfield: the shift register's phase and the speed chosen by the program. */
typedef struct {
    int32_t phase;
    int32_t speed;
} StarField;

/** A star on the screen in this frame: position and colour (BBGGRR, 6 bits). */
typedef struct {
    uint8_t x;
    uint16_t y;
    uint8_t color;
} Star;

/** The register's position (steps since the seed) at the start of the next frame. */
GAME_PRIVATE int32_t advance_stars(int32_t phase, int32_t speed);

/**
 * The two sets shown in a frame: the program changes them every 4 frames, from bits 2-4 of its
 * frame counter (`jp_Task_man`); the hardware always shows one of sets 0-1 and one of 2-3.
 */
GAME_PRIVATE void star_sets(int32_t frame, int32_t sets[2]);

/**
 * The stars of a frame, written to `out` (room for STAR_COUNT); returns how many. The chip draws
 * its raster along the monitor's long side, which the cabinet turns upright: a raster line is a
 * screen column, from right to left.
 */
GAME_PRIVATE int32_t visible_stars(int32_t phase, int32_t speed, int32_t frame, Star *out);

#endif
