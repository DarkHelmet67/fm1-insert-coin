/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * A screen at half size, for the launcher's menu: each 2 x 2 block of pixels becomes one.
 */
#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <stdint.h>

#include "screen/framebuffer.h"

/**
 * The colour a 2 x 2 block shrinks to: its first pixel that is not background, reading left to
 * right, top to bottom. Averaging is not possible with colour indexes, and taking always the same
 * corner would lose the thin things (a shot one pixel wide, the lines of a letter).
 * @param src the screen
 * @param x   the block's left column
 * @param y   the block's top row
 */
uint8_t thumbnail_block(const Framebuffer *src, int32_t x, int32_t y);

/**
 * Draws `src` at half size into `dst`, its top left corner at (x, y), clipped to `dst`.
 * The colour indexes are copied as they are: `dst` must use the same palette for them.
 */
void thumbnail_draw(Framebuffer *dst, const Framebuffer *src, int32_t x, int32_t y);

#endif
