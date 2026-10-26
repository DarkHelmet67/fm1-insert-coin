/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The game's own screen, at the original resolution: one byte per pixel, a colour index (0 is the
 * background). Drawing only changes bytes; the FM-1's display gets the rows that changed, so a
 * frame where one invader moves costs a few rows of SPI, not the whole screen.
 */
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

/** The widest screen of the games (Breakout, 228 pixels), rounded up to the FM-1's. */
#define FB_MAX_WIDTH 240u
/** The tallest screen of the games (Galaga, 288 rows). */
#define FB_MAX_HEIGHT 288u
/** Colour indexes run from 0 to FB_COLORS - 1 (Galaga: 32 colours and 64 for its stars). */
#define FB_COLORS 128u

/** An RGB565 colour from 8-bit red, green and blue, as the display takes it (before byte swapping).
 */
#define RGB565(r, g, b) ((uint16_t)((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3)))

/** A screen of colour indexes, with one "changed" bit per row. */
typedef struct {
    uint16_t width;
    uint16_t height;
    uint32_t dirty[(FB_MAX_HEIGHT + 31u) / 32u]; /**< bit y: row y changed since the last push */
    uint8_t pixels[FB_MAX_WIDTH * FB_MAX_HEIGHT];
} Framebuffer;

/**
 * Sets the size, clears every pixel to 0 and marks every row as changed.
 * @param fb     the screen (about 70 KB: on the FM-1 it lives in the memory pool)
 * @param width  at most FB_MAX_WIDTH
 * @param height at most FB_MAX_HEIGHT
 */
void fb_init(Framebuffer *fb, uint32_t width, uint32_t height);

/** The colour index at (x, y); 0 outside the screen. */
uint8_t fb_get(const Framebuffer *fb, int32_t x, int32_t y);

/** Sets one pixel; outside the screen it does nothing. Marks the row only if the pixel changes. */
void fb_set(Framebuffer *fb, int32_t x, int32_t y, uint8_t color);

/** Fills a rectangle, clipped to the screen. */
void fb_fill_rect(Framebuffer *fb, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t color);

/** Whether row y changed since the last fb_clear_dirty(). */
int fb_row_dirty(const Framebuffer *fb, int32_t y);

/** Marks every row as changed: after a palette change, every pixel looks different. */
void fb_mark_all_dirty(Framebuffer *fb);

/** Forgets the changes: call it once the rows have been sent to the display. */
void fb_clear_dirty(Framebuffer *fb);

/**
 * Brings `front` (what the display shows) up to date with `back` (the frame just drawn):
 * copies only the rows that differ and marks them changed. A game redraws its whole frame in
 * `back` every time, and only the rows that really changed travel to the display.
 * @param front the screen that is pushed to the display; same size as `back`
 * @param back the frame drawn from scratch
 */
void fb_present(Framebuffer *front, const Framebuffer *back);

#endif
