/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen_push.h"

/** Whether display row y shows a game row that changed. */
static int display_row_dirty(const Framebuffer *fb, const ScreenMap *map, uint32_t y)
{
    const int16_t row = map->source_row[y];
    return row != SCREEN_NO_ROW && fb_row_dirty(fb, row);
}

ScreenBand screen_next_band(const Framebuffer *fb, const ScreenMap *map, uint32_t from,
                            uint32_t max_rows)
{
    ScreenBand band = {0, 0};
    uint32_t y = from;
    while (y < SCREEN_SIZE && !display_row_dirty(fb, map, y))
        y++;
    if (y == SCREEN_SIZE)
        return band;
    band.y = (uint16_t)y;
    while (y < SCREEN_SIZE && band.count < max_rows && display_row_dirty(fb, map, y)) {
        band.count++;
        y++;
    }
    return band;
}

void screen_render_row(const Framebuffer *fb, const ScreenMap *map, uint32_t y,
                       const uint16_t palette[FB_COLORS], uint16_t *out)
{
    const int16_t row = map->source_row[y];
    for (uint32_t i = 0; i < map->width; i++) {
        const uint8_t color = row == SCREEN_NO_ROW ? 0u : fb_get(fb, map->source_x + i, row);
        out[i] = palette[color % FB_COLORS];
    }
}
