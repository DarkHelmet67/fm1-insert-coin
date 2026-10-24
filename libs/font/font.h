/* SPDX-License-Identifier: GPL-3.0-only */
/** @file Bitmap fonts: every character is a small sprite of the same size (insert-coin, font.ts).
 */
#ifndef FONT_H
#define FONT_H

#include "screen/framebuffer.h"
#include "sprite/sprite.h"

/** A bitmap font. */
typedef struct {
    uint8_t glyph_width;
    uint8_t glyph_height;
    uint8_t spacing;             /**< empty pixels between two characters */
    const char *chars;           /**< the characters the font has, in the order of `glyphs` */
    const Sprite *const *glyphs; /**< one sprite per character of `chars` */
} PixelFont;

/** The glyph of `c`, or NULL when the font does not have it; lowercase reads as uppercase. */
const Sprite *font_glyph(const PixelFont *font, char c);

/** Width in pixels of `text` written with `font`. */
int32_t font_text_width(const PixelFont *font, const char *text);

/** Writes `text` with its top-left corner at (x, y); unknown characters are left blank. */
void fb_draw_text(Framebuffer *fb, const PixelFont *font, const char *text, int32_t x, int32_t y,
                  uint8_t color);

/** Writes `text` centred on the screen's width, with its top at y. */
void fb_draw_text_centered(Framebuffer *fb, const PixelFont *font, const char *text, int32_t y,
                           uint8_t color);

#endif
