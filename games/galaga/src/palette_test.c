/* SPDX-License-Identifier: GPL-3.0-only */
#include "palette.h"
#include "test/check.h"

static uint16_t colors[FB_COLORS];

static void the_fighter_palette_9_is_red_blue_and_white(void)
{
    galaga_colors(colors);
    const TileColors fighter = sprite_colors(9);
    CHECK_EQ(colors[fighter.index[0]], RGB565(0xff, 0x00, 0x00));
    CHECK_EQ(colors[fighter.index[1]], prom_rgb565(0xd8));
    CHECK_EQ(colors[fighter.index[2]], prom_rgb565(0xf6));
}

static void fighter_captured_palette_4_is_written_in_red(void)
{
    galaga_colors(colors);
    CHECK_EQ(colors[char_colors(4).index[2]], RGB565(0xff, 0x00, 0x00));
}

static void pixels_on_colour_15_are_see_through(void)
{
    const TileColors c = char_colors(0x1b);
    CHECK_EQ(c.index[0], SEE_THROUGH_INDEX);
    CHECK_EQ(c.index[1], SEE_THROUGH_INDEX);
    CHECK_EQ(colors[c.index[2]], RGB565(0xff, 0x00, 0x00));
}

static void the_prom_weights_give_full_white_and_black(void)
{
    CHECK_EQ(prom_rgb565(0xff), RGB565(0xff, 0xff, 0xff));
    CHECK_EQ(prom_rgb565(0x00), 0);
    CHECK_EQ(colors[COLOR_BACKGROUND], 0);
}

int main(void)
{
    RUN(the_fighter_palette_9_is_red_blue_and_white);
    RUN(fighter_captured_palette_4_is_written_in_red);
    RUN(pixels_on_colour_15_are_see_through);
    RUN(the_prom_weights_give_full_white_and_black);
    return check_report();
}
