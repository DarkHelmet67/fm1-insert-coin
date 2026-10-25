/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Drawing the tiles of the ROMs (sprite-rom.ts, draw-objects.ts): 16 x 16 sprites, mirrored as
 * the hardware does, double size and double width sprites, and 8 x 8 characters. A tile pixel
 * is a value 0-3: 0 is never drawn, 1-3 take their colour from the tile's palette.
 */
#ifndef GALAGA_TILES_H
#define GALAGA_TILES_H

#include <stdint.h>

#include "cartridge/private.h"
#include "objects.h"
#include "palette.h"
#include "screen/framebuffer.h"

/** Pixel value (0-3) of sprite `code` at (x, y), 0-15, as stored. */
GAME_PRIVATE int32_t sprite_pixel(int32_t code, int32_t x, int32_t y);

/**
 * Pixel value of the double size sprite made of codes `code` to `code + 3`, at (x, y), 0-31. On
 * the vertical monitor tile `code` sits top right, `code + 1` bottom right, `code + 2` top left
 * and `code + 3` bottom left: the hardware lays them out along the monitor's own axes, and the
 * cabinet turns it.
 */
GAME_PRIVATE int32_t big_sprite_pixel(int32_t code, int32_t x, int32_t y);

/** Pixel value of character `code` at (x, y), 0-7. */
GAME_PRIVATE int32_t char_pixel(int32_t code, int32_t x, int32_t y);

/** Draws character `code` at the cell (column, row) of the 28 x 36 grid. */
GAME_PRIVATE void draw_char(Framebuffer *fb, int32_t code, int32_t column, int32_t row,
                            const TileColors *colors);

/** Draws one object's sprite, as the sprite hardware places it. */
GAME_PRIVATE void draw_object(Framebuffer *fb, const GameObject *object);

/**
 * Draws all objects in table order (later ones on top, like the hardware). An object at x = 0 is
 * off screen: the program hides sprites that way.
 */
GAME_PRIVATE void draw_objects(Framebuffer *fb, const Objects *objects);

#endif
