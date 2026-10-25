/* SPDX-License-Identifier: GPL-3.0-only */
#include "thumbnail.h"

uint8_t thumbnail_block(const Framebuffer *src, int32_t x, int32_t y)
{
    const uint8_t pixels[4] = {fb_get(src, x, y), fb_get(src, x + 1, y), fb_get(src, x, y + 1),
                               fb_get(src, x + 1, y + 1)};
    for (uint32_t i = 0; i < 4u; i++)
        if (pixels[i] != 0u)
            return pixels[i];
    return 0u;
}

void thumbnail_draw(Framebuffer *dst, const Framebuffer *src, int32_t x, int32_t y)
{
    const int32_t width = src->width / 2;
    const int32_t height = src->height / 2;
    for (int32_t row = 0; row < height; row++)
        for (int32_t col = 0; col < width; col++)
            fb_set(dst, x + col, y + row, thumbnail_block(src, col * 2, row * 2));
}
