/* SPDX-License-Identifier: GPL-3.0-only */
#include "palette.h"
#include "screen/draw.h"
#include "test/check.h"
#include "text.h"

static Framebuffer fb;

static void digits_letters_and_symbols_map_to_the_rom_font(void)
{
    CHECK_EQ(char_code('0'), 0);
    CHECK_EQ(char_code('9'), 9);
    CHECK_EQ(char_code('A'), 0x0a);
    CHECK_EQ(char_code('Z'), 0x23);
    CHECK_EQ(char_code('-'), 0x26);
    CHECK_EQ(char_code('?'), 0x24); /* not in the font: a space */
    CHECK_EQ(char_code((char)0xa9), 0x2e);
}

static void text_is_drawn_in_its_cells(void)
{
    fb_init(&fb, 224, 288);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_cell_text(&fb, "1", 2, 3, 4);
    int lit = 0;
    for (int y = 24; y < 32; y++)
        for (int x = 16; x < 24; x++)
            lit += fb_get(&fb, x, y) != COLOR_BACKGROUND;
    CHECK(lit > 0);
    CHECK_EQ(fb_get(&fb, 8, 24), COLOR_BACKGROUND); /* the cell on its left stays empty */
}

int main(void)
{
    RUN(digits_letters_and_symbols_map_to_the_rom_font);
    RUN(text_is_drawn_in_its_cells);
    return check_report();
}
