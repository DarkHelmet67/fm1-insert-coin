/* SPDX-License-Identifier: GPL-3.0-only */
/** @file Drawing sprites and bitmaps into a framebuffer, in one colour. */
#ifndef DRAW_H
#define DRAW_H

#include "screen/framebuffer.h"
#include "sprite/bitmap.h"
#include "sprite/sprite.h"

/** Draws the lit pixels of `sprite` with its top-left corner at (x, y); the rest stays as it is. */
void fb_draw_sprite(Framebuffer *fb, const Sprite *sprite, int32_t x, int32_t y, uint8_t color);

/** Draws the lit pixels of `bitmap` with its top-left corner at (x, y). */
void fb_draw_bitmap(Framebuffer *fb, const Bitmap *bitmap, int32_t x, int32_t y, uint8_t color);

/** Fills the whole screen with one colour. */
void fb_clear(Framebuffer *fb, uint8_t color);

#endif
