/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen_map.h"

/** A map with no rows and the game's columns centred (cropped evenly when wider). */
static ScreenMap empty_map(uint32_t width)
{
    ScreenMap map;
    for (uint32_t y = 0; y < SCREEN_SIZE; y++)
        map.source_row[y] = SCREEN_NO_ROW;
    const uint32_t shown = width < SCREEN_SIZE ? width : SCREEN_SIZE;
    map.width = (uint16_t)shown;
    map.source_x = (uint16_t)((width - shown) / 2u);
    map.dest_x = (uint16_t)((SCREEN_SIZE - shown) / 2u);
    return map;
}

ScreenMap screen_map_centered(uint32_t width, uint32_t height)
{
    const uint32_t shown = height < SCREEN_SIZE ? height : SCREEN_SIZE;
    const RowRange all = {(uint16_t)((height - shown) / 2u),
                          (uint16_t)((height - shown) / 2u + shown - 1u)};
    return screen_map_rows(width, &all, 1u);
}

ScreenMap screen_map_rows(uint32_t width, const RowRange *ranges, uint32_t count)
{
    ScreenMap map = empty_map(width);
    uint32_t total = 0;
    for (uint32_t i = 0; i < count; i++)
        total += (uint32_t)(ranges[i].last - ranges[i].first + 1u);
    uint32_t y = total < SCREEN_SIZE ? (SCREEN_SIZE - total) / 2u : 0u;
    for (uint32_t i = 0; i < count; i++)
        for (uint32_t row = ranges[i].first; row <= ranges[i].last && y < SCREEN_SIZE; row++)
            map.source_row[y++] = (int16_t)row;
    return map;
}
