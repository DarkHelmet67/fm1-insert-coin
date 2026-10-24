/* SPDX-License-Identifier: GPL-3.0-only */
#include "framebuffer.h"

/** Whether (x, y) is on the screen. */
static int inside(const Framebuffer *fb, int32_t x, int32_t y)
{
    return x >= 0 && y >= 0 && x < (int32_t)fb->width && y < (int32_t)fb->height;
}

/** Marks row y as changed (y must be on the screen). */
static void mark_row(Framebuffer *fb, int32_t y)
{
    fb->dirty[y / 32] |= 1u << (y % 32);
}

/** The smaller of two numbers. */
static int32_t min_i32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

/** The larger of two numbers. */
static int32_t max_i32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

void fb_init(Framebuffer *fb, uint32_t width, uint32_t height)
{
    fb->width = (uint16_t)(width < FB_MAX_WIDTH ? width : FB_MAX_WIDTH);
    fb->height = (uint16_t)(height < FB_MAX_HEIGHT ? height : FB_MAX_HEIGHT);
    for (uint32_t i = 0; i < FB_MAX_WIDTH * FB_MAX_HEIGHT; i++)
        fb->pixels[i] = 0;
    fb_mark_all_dirty(fb);
}

uint8_t fb_get(const Framebuffer *fb, int32_t x, int32_t y)
{
    return inside(fb, x, y) ? fb->pixels[y * fb->width + x] : 0u;
}

void fb_set(Framebuffer *fb, int32_t x, int32_t y, uint8_t color)
{
    if (!inside(fb, x, y) || fb->pixels[y * fb->width + x] == color)
        return;
    fb->pixels[y * fb->width + x] = color;
    mark_row(fb, y);
}

void fb_fill_rect(Framebuffer *fb, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t color)
{
    const int32_t x0 = max_i32(x, 0), x1 = min_i32(x + w, fb->width);
    const int32_t y0 = max_i32(y, 0), y1 = min_i32(y + h, fb->height);
    for (int32_t row = y0; row < y1; row++)
        for (int32_t col = x0; col < x1; col++)
            fb_set(fb, col, row, color);
}

int fb_row_dirty(const Framebuffer *fb, int32_t y)
{
    return inside(fb, 0, y) && (fb->dirty[y / 32] >> (y % 32) & 1u);
}

void fb_mark_all_dirty(Framebuffer *fb)
{
    for (uint32_t i = 0; i < sizeof fb->dirty / sizeof fb->dirty[0]; i++)
        fb->dirty[i] = 0xFFFFFFFFu;
}

void fb_clear_dirty(Framebuffer *fb)
{
    for (uint32_t i = 0; i < sizeof fb->dirty / sizeof fb->dirty[0]; i++)
        fb->dirty[i] = 0;
}
