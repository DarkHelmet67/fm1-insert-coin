/* SPDX-License-Identifier: GPL-3.0-only */
#include "font/arcade_font.h"
#include "test/check.h"

static Framebuffer fb;

static void every_glyph_is_5_by_7(void)
{
    for (uint32_t i = 0; ARCADE_FONT.chars[i]; i++) {
        CHECK_EQ(ARCADE_FONT.glyphs[i]->width, 5);
        CHECK_EQ(ARCADE_FONT.glyphs[i]->height, 7);
        CHECK(sprite_is_well_formed(ARCADE_FONT.glyphs[i]));
    }
}

static void lowercase_reads_as_uppercase_and_unknown_is_null(void)
{
    CHECK(font_glyph(&ARCADE_FONT, 'a') == font_glyph(&ARCADE_FONT, 'A'));
    CHECK(font_glyph(&ARCADE_FONT, '#') == 0);
}

static void text_width_counts_the_spacing_between_characters(void)
{
    CHECK_EQ(font_text_width(&ARCADE_FONT, ""), 0);
    CHECK_EQ(font_text_width(&ARCADE_FONT, "A"), 5);
    CHECK_EQ(font_text_width(&ARCADE_FONT, "0120"), 4 * 8 - 3);
}

static void text_is_drawn_character_by_character(void)
{
    fb_init(&fb, 64u, 16u);
    fb_draw_text(&fb, &ARCADE_FONT, "-1", 0, 0, 4);
    CHECK_EQ(fb_get(&fb, 0, 3), 4); /* the dash's middle row */
    CHECK_EQ(fb_get(&fb, 0, 0), 0);
    CHECK_EQ(fb_get(&fb, 10, 0), 4); /* the top of the 1, one glyph and 3 pixels later */
}

static void centred_text_sits_in_the_middle(void)
{
    fb_init(&fb, 64u, 16u);
    fb_draw_text_centered(&fb, &ARCADE_FONT, "-", 2, 4);
    CHECK_EQ(fb_get(&fb, 29, 5), 4);
    CHECK_EQ(fb_get(&fb, 33, 5), 4);
    CHECK_EQ(fb_get(&fb, 28, 5), 0);
}

int main(void)
{
    RUN(every_glyph_is_5_by_7);
    RUN(lowercase_reads_as_uppercase_and_unknown_is_null);
    RUN(text_width_counts_the_spacing_between_characters);
    RUN(text_is_drawn_character_by_character);
    RUN(centred_text_sits_in_the_middle);
    return check_report();
}
