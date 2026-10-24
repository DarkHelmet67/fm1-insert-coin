/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Hardware glue: sends the game's screen to the FM-1's display. Included by a game's main.c
 * after platform.c (it calls lcd_fill and lcd_blit), never compiled on its own.
 *
 * The changed rows go out in bands of up to SCREEN_BAND_ROWS rows. lcd_blit() returns while the
 * DMA is still reading its buffer, so there are two buffers: the next band is coloured in one
 * while the other is on its way. The next lcd_blit() waits for the previous transfer before it
 * starts, so a buffer is never rewritten while it is being sent.
 */
#ifndef SCREEN_FM1_H
#define SCREEN_FM1_H

#include "screen/framebuffer.h"
#include "screen/screen_map.h"
#include "screen/screen_push.h"

/** The rows of one band: 240 x 32 pixels, 15 KB a buffer. */
#define SCREEN_BAND_ROWS 32u

/** The two band buffers, in the memory pool. */
static uint16_t screen_band_px[2][SCREEN_SIZE * SCREEN_BAND_ROWS] __attribute__((section(".pool")));

/**
 * Turns RGB565 colours into the byte order the display takes (gfx.c's swap16).
 * @param rgb565 FB_COLORS colours as RGB(r, g, b) makes them
 * @param out    the same colours for screen_push()
 */
static void screen_palette(const uint16_t rgb565[FB_COLORS], uint16_t out[FB_COLORS])
{
    for (uint32_t i = 0; i < FB_COLORS; i++)
        out[i] = swap16(rgb565[i]);
}

/** Clears the whole display to black: the margins around the game are never drawn again. */
static void screen_clear(void)
{
    lcd_fill(0, 0, SCREEN_SIZE, SCREEN_SIZE, C_BLACK);
}

/**
 * Sends the rows that changed since the last call, then forgets the changes.
 * @param fb      the game's screen
 * @param map     where it lands on the display
 * @param palette FB_COLORS colours from screen_palette()
 */
static void screen_push(Framebuffer *fb, const ScreenMap *map, const uint16_t palette[FB_COLORS])
{
    uint32_t which = 0;
    for (ScreenBand band = screen_next_band(fb, map, 0u, SCREEN_BAND_ROWS); band.count;
         band = screen_next_band(fb, map, band.y + band.count, SCREEN_BAND_ROWS)) {
        uint16_t *px = screen_band_px[which];
        for (uint32_t r = 0; r < band.count; r++)
            screen_render_row(fb, map, band.y + r, palette, &px[r * map->width]);
        lcd_blit(map->dest_x, band.y, map->width, band.count, px);
        which ^= 1u;
    }
    fb_clear_dirty(fb);
}

#endif
