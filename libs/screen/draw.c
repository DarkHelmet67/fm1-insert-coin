/* SPDX-License-Identifier: GPL-3.0-only */
#include "draw.h"

void fb_draw_sprite(Framebuffer *fb, const Sprite *sprite, int32_t x, int32_t y, uint8_t color)
{
    for (int32_t row = 0; row < sprite->height; row++)
        for (int32_t col = 0; col < sprite->width; col++)
            if (sprite_lit(sprite, col, row))
                fb_set(fb, x + col, y + row, color);
}

void fb_draw_bitmap(Framebuffer *fb, const Bitmap *bitmap, int32_t x, int32_t y, uint8_t color)
{
    for (int32_t row = 0; row < bitmap->height; row++)
        for (int32_t col = 0; col < bitmap->width; col++)
            if (bitmap_lit(bitmap, col, row))
                fb_set(fb, x + col, y + row, color);
}

void fb_clear(Framebuffer *fb, uint8_t color)
{
    fb_fill_rect(fb, 0, 0, fb->width, fb->height, color);
}
