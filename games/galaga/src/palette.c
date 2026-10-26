/* SPDX-License-Identifier: GPL-3.0-only */
#include "palette.h"

#include "rom.h"
#include "table.h"

/**
 * Brightness added by each bit of a red or green channel. Namco's colour PROM drives three
 * resistors (1 kOhm, 470 Ohm, 220 Ohm) for red and green, two (470 Ohm, 220 Ohm) for blue: these
 * are the weights MAME computes from those resistors.
 */
static const uint8_t RED_GREEN_WEIGHTS[3] = {0x21, 0x47, 0x97};
static const uint8_t BLUE_WEIGHTS[2] = {0x51, 0xae};

/** Brightness of each 2-bit colour channel of a star, from the resistors of its DAC (MAME). */
static const uint8_t STAR_LEVELS[4] = {0x00, 0x47, 0x97, 0xde};

/** The screen colour that is "see-through": pixels on it are not drawn. */
#define SEE_THROUGH 0x0f

/** Sum of the weights whose bit is set in `bits`. */
static int32_t channel(int32_t bits, const uint8_t *weights, int32_t count)
{
    int32_t sum = 0;
    for (int32_t bit = 0; bit < count; bit++)
        sum += ((bits >> bit) & 1) * weights[bit];
    return sum;
}

uint16_t prom_rgb565(int32_t byte)
{
    return RGB565(channel(byte & 7, RED_GREEN_WEIGHTS, 3),
                  channel((byte >> 3) & 7, RED_GREEN_WEIGHTS, 3),
                  channel((byte >> 6) & 3, BLUE_WEIGHTS, 2));
}

uint16_t star_rgb565(int32_t color)
{
    return RGB565(STAR_LEVELS[color & 3], STAR_LEVELS[(color >> 2) & 3],
                  STAR_LEVELS[(color >> 4) & 3]);
}

void galaga_colors(uint16_t out[FB_COLORS])
{
    for (uint32_t i = 0; i < FB_COLORS; i++)
        out[i] = 0;
    for (uint32_t c = 0; c < PALETTE_PROM_SIZE; c++)
        out[SCREEN_COLOR_BASE + c] = prom_rgb565(PALETTE_PROM[c]);
    for (int32_t c = 0; c < 64; c++)
        out[STAR_COLOR_BASE + c] = star_rgb565(c);
}

/** The colours of palette `index` of a lookup PROM pointing into screen colours `first`... */
static TileColors lookup(const uint8_t *prom, uint32_t size, int32_t index, int32_t first)
{
    TileColors colors;
    for (int32_t value = 1; value <= 3; value++) {
        int32_t entry = table_at(prom, size, index * 4 + value, 0) & 0x0f;
        colors.index[value - 1] =
            (uint8_t)(entry == SEE_THROUGH ? SEE_THROUGH_INDEX : SCREEN_COLOR_BASE + first + entry);
    }
    return colors;
}

TileColors sprite_colors(int32_t index)
{
    return lookup(SPRITE_LOOKUP_PROM, SPRITE_LOOKUP_PROM_SIZE, index, 0);
}

TileColors char_colors(int32_t index)
{
    return lookup(CHAR_LOOKUP_PROM, CHAR_LOOKUP_PROM_SIZE, index, 16);
}
