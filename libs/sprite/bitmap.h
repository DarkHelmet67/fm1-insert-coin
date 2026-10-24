/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * A one-bit picture that can change (a shield crumbling under the shots), and pixel-perfect
 * collisions between pictures placed on the screen, as @arcade/collision does on the web.
 */
#ifndef BITMAP_H
#define BITMAP_H

#include <stdint.h>

#include "sprite.h"

/** The tallest bitmap: a shield is 16 rows. */
#define BITMAP_MAX_ROWS 16u
/** The widest bitmap: one bit per pixel in a 32-bit row. */
#define BITMAP_MAX_WIDTH 32u

/** A picture of up to 32 x 16 pixels; in each row, bit 31 is the leftmost pixel. */
typedef struct {
    uint8_t width;
    uint8_t height;
    uint32_t rows[BITMAP_MAX_ROWS];
} Bitmap;

/** A bitmap with its top-left corner at (x, y) on the screen. */
typedef struct {
    const Bitmap *bitmap;
    int32_t x;
    int32_t y;
} Placed;

/** The bitmap of a sprite (cropped to 32 x 16). */
Bitmap bitmap_from_sprite(const Sprite *sprite);

/** Whether the pixel at (x, y) of the bitmap is lit; outside it is not. */
int bitmap_lit(const Bitmap *bitmap, int32_t x, int32_t y);

/** How many pixels are lit. */
uint32_t bitmap_count(const Bitmap *bitmap);

/**
 * Pixel-perfect collision: whether two placed bitmaps have a lit pixel in the same place. Like
 * the web version, bitmaps that only share an edge do not collide.
 */
int placed_overlap(const Placed *a, const Placed *b);

/**
 * Destructible terrain: the target's bitmap with every pixel under a lit pixel of the brush
 * switched off (a shot carving a hole in a shield).
 */
Bitmap bitmap_erase(const Placed *target, const Placed *brush);

#endif
