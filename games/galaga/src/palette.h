/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The colours (palette.ts): the 32 colours of the screen from the colour PROM, the 4 colours of
 * each sprite and character palette from the lookup PROMs, and the 64 colours a star can have.
 * They become the framebuffer's colour indexes: 0 is the black background, then the 32 screen
 * colours, then the 64 star colours.
 */
#ifndef GALAGA_PALETTE_H
#define GALAGA_PALETTE_H

#include <stdint.h>

#include "cartridge/private.h"
#include "screen/framebuffer.h"

/** The framebuffer's colour indexes. */
enum {
    COLOR_BACKGROUND = 0,
    SCREEN_COLOR_BASE = 1, /**< screen colour c is index 1 + c (32 of them) */
    STAR_COLOR_BASE = 33,  /**< star colour c (BBGGRR) is index 33 + c (64 of them) */
    GALAGA_COLORS = 97,    /**< indexes used */
};

/** A pixel value that is not drawn: what lies below stays visible. */
#define SEE_THROUGH_INDEX 0xff

/** The framebuffer colours of the 3 pixel values (1-3) of a tile in one palette. */
typedef struct {
    uint8_t index[3]; /**< a colour index, or SEE_THROUGH_INDEX */
} TileColors;

/** The RGB565 colour of a colour PROM byte, BBGGGRRR, with the weights of its resistors. */
GAME_PRIVATE uint16_t prom_rgb565(int32_t byte);

/** The RGB565 colour of a star, 2 bits per channel: BBGGRR. */
GAME_PRIVATE uint16_t star_rgb565(int32_t color);

/** All the game's colours, at their indexes (the rest of the FB_COLORS are black). */
GAME_PRIVATE void galaga_colors(uint16_t out[FB_COLORS]);

/** Sprite palette `index` (0-15): its colours come from screen colours 0-15. */
GAME_PRIVATE TileColors sprite_colors(int32_t index);

/** Character palette `index` (0-31): characters use screen colours 16-31. */
GAME_PRIVATE TileColors char_colors(int32_t index);

#endif
