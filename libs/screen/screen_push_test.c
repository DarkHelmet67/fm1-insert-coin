/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/screen_push.h"
#include "test/check.h"

static Framebuffer fb;
static const uint16_t PALETTE[FB_COLORS] = {0x0000u, 0xFFFFu, 0x07E0u, 0xF800u};

/** A 224 x 240 screen, centred, with no row changed. */
static ScreenMap quiet_screen(void)
{
    fb_init(&fb, 224u, 240u);
    fb_clear_dirty(&fb);
    return screen_map_centered(224u, 240u);
}

static void nothing_changed_nothing_to_send(void)
{
    const ScreenMap map = quiet_screen();
    CHECK_EQ(screen_next_band(&fb, &map, 0u, 64u).count, 0u);
}

static void changed_rows_make_a_band(void)
{
    const ScreenMap map = quiet_screen();
    fb_fill_rect(&fb, 0, 100, 8, 3, 1u);
    const ScreenBand band = screen_next_band(&fb, &map, 0u, 64u);
    CHECK_EQ(band.y, 100u);
    CHECK_EQ(band.count, 3u);
    CHECK_EQ(screen_next_band(&fb, &map, 103u, 64u).count, 0u);
}

static void a_band_is_no_longer_than_the_buffer(void)
{
    const ScreenMap map = quiet_screen();
    fb_mark_all_dirty(&fb);
    const ScreenBand first = screen_next_band(&fb, &map, 0u, 64u);
    const ScreenBand last = screen_next_band(&fb, &map, 192u, 64u);
    CHECK_EQ(first.y, 0u);
    CHECK_EQ(first.count, 64u);
    CHECK_EQ(last.y, 192u);
    CHECK_EQ(last.count, 48u);
}

static void separate_changes_make_separate_bands(void)
{
    const ScreenMap map = quiet_screen();
    fb_set(&fb, 0, 10, 1u);
    fb_set(&fb, 0, 50, 1u);
    const ScreenBand first = screen_next_band(&fb, &map, 0u, 64u);
    const ScreenBand second = screen_next_band(&fb, &map, first.y + first.count, 64u);
    CHECK_EQ(first.y, 10u);
    CHECK_EQ(first.count, 1u);
    CHECK_EQ(second.y, 50u);
}

static void a_row_gets_its_colours_from_the_palette(void)
{
    const ScreenMap map = quiet_screen();
    uint16_t out[240];
    fb_set(&fb, 0, 5, 2u);
    fb_set(&fb, 223, 5, 3u);
    screen_render_row(&fb, &map, 5u, PALETTE, out);
    CHECK_EQ(out[0], 0x07E0u);
    CHECK_EQ(out[1], 0x0000u);
    CHECK_EQ(out[223], 0xF800u);
}

static void a_row_with_no_game_row_is_background(void)
{
    fb_init(&fb, 224u, 100u);
    const ScreenMap map = screen_map_centered(224u, 100u);
    uint16_t out[240];
    fb_fill_rect(&fb, 0, 0, 224, 100, 1u);
    screen_render_row(&fb, &map, 0u, PALETTE, out);
    CHECK_EQ(out[0], 0x0000u);
    CHECK_EQ(screen_next_band(&fb, &map, 0u, 64u).y, 70u);
}

int main(void)
{
    RUN(nothing_changed_nothing_to_send);
    RUN(changed_rows_make_a_band);
    RUN(a_band_is_no_longer_than_the_buffer);
    RUN(separate_changes_make_separate_bands);
    RUN(a_row_gets_its_colours_from_the_palette);
    RUN(a_row_with_no_game_row_is_background);
    return check_report();
}
