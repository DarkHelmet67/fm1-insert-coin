/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Sprites drawn by hand as text, one string per pixel row, 'X' for a lit pixel: the same pictures
 * as the web version, readable in the source, with no image files.
 */
#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>

/** A picture made of rows of text: 'X' is lit, anything else is not. */
typedef struct {
    uint8_t width;
    uint8_t height;
    const char *const *rows;
} Sprite;

/**
 * Defines the Sprite `name` from its rows; the height is counted, the width is given (and
 * checked by sprite_is_well_formed() in the tests):
 *
 *     SPRITE_DEFINE(SHOT, 1, "X", "X", "X", "X");
 */
#define SPRITE_DEFINE(name, w, ...)                                                                \
    static const char *const name##_ROWS[] = {__VA_ARGS__};                                        \
    const Sprite name = {(w), (uint8_t)(sizeof name##_ROWS / sizeof name##_ROWS[0]), name##_ROWS}

/** Whether the pixel at (x, y) is lit; outside the sprite it is not. */
int sprite_lit(const Sprite *sprite, int32_t x, int32_t y);

/** Whether every row is exactly `width` characters long, as parseSprite checks on the web. */
int sprite_is_well_formed(const Sprite *sprite);

#endif
