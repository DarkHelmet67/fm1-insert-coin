/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * From the game's screen to the display: which display rows need sending, grouped in bands, and
 * the colours of one display row.
 *
 * The display takes about 31 ms for a full screen, more than a 60 Hz frame (16.7 ms), so only
 * changed rows go out. A band is a run of consecutive changed rows, as many as fit the buffer the
 * caller sends in one transfer.
 */
#ifndef SCREEN_PUSH_H
#define SCREEN_PUSH_H

#include <stdint.h>

#include "framebuffer.h"
#include "screen_map.h"

/** A run of display rows to send: rows y to y + count - 1. count 0: nothing left. */
typedef struct {
    uint16_t y;
    uint16_t count;
} ScreenBand;

/**
 * The next band of changed display rows, at or below row `from`.
 * @param fb       the game's screen, with its changed rows
 * @param map      where it lands on the display
 * @param from     the first display row to look at
 * @param max_rows the most rows the caller can send at once
 */
ScreenBand screen_next_band(const Framebuffer *fb, const ScreenMap *map, uint32_t from,
                            uint32_t max_rows);

/**
 * The colours of the game's columns on one display row.
 * @param fb      the game's screen
 * @param map     where it lands
 * @param y       the display row
 * @param palette FB_COLORS colours, in the display's format (the caller swaps bytes if needed)
 * @param out     map->width pixels
 */
void screen_render_row(const Framebuffer *fb, const ScreenMap *map, uint32_t y,
                       const uint16_t palette[FB_COLORS], uint16_t *out);

#endif
