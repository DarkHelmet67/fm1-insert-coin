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
 *
 * screen_push_band() sends one band at a time, so the main loop can run the game's frames on time
 * between two bands: a whole screen takes longer than a frame (guide 02).
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

/** The band buffer to colour next, and the display row the next band is looked for from. */
static uint32_t screen_which, screen_from;

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
    screen_from = 0;
    lcd_fill(0, 0, SCREEN_SIZE, SCREEN_SIZE, C_BLACK);
}

/**
 * Sends the next band of the rows that changed; once there is none left, forgets the changes.
 * @param fb      the game's screen
 * @param map     where it lands on the display
 * @param palette FB_COLORS colours from screen_palette()
 * @returns whether a band was sent (rows may still be waiting)
 */
static int screen_push_band(Framebuffer *fb, const ScreenMap *map,
                            const uint16_t palette[FB_COLORS])
{
    const ScreenBand band = screen_next_band(fb, map, screen_from, SCREEN_BAND_ROWS);
    if (band.count == 0) {
        fb_clear_dirty(fb);
        screen_from = 0;
        return 0;
    }
    uint16_t *px = screen_band_px[screen_which];
    for (uint32_t r = 0; r < band.count; r++)
        screen_render_row(fb, map, band.y + r, palette, &px[r * map->width]);
    lcd_blit(map->dest_x, band.y, map->width, band.count, px);
    screen_which ^= 1u;
    screen_from = band.y + band.count;
    return 1;
}

#endif
