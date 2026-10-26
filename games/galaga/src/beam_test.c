/* SPDX-License-Identifier: GPL-3.0-only */
#include "beam.h"
#include "capture_state.h"
#include "palette.h"
#include "screen/draw.h"
#include "test/check.h"

static Framebuffer fb;

/** Pixels of rows `first` to `last` (character rows) that are not background. */
static int32_t lit_in_rows(int32_t first, int32_t last)
{
    int32_t n = 0;
    for (int32_t y = first * 8; y < last * 8 + 8; y++)
        for (int32_t x = 0; x < 224; x++)
            n += fb_get(&fb, x, y) != COLOR_BACKGROUND;
    return n;
}

static void the_beam_column_follows_the_bosses_x(void)
{
    CHECK_EQ(beam_column(0x10), 29);
    CHECK(beam_column(0x80) < beam_column(0xc0));
}

static void the_beam_palette_cycles_every_4_frames(void)
{
    CHECK_EQ(beam_palette(0), 0x18);
    CHECK_EQ(beam_palette(4), 0x18);
    CHECK_EQ(beam_palette(8), 0x19);
    CHECK_EQ(beam_palette(12), 0x1a);
}

static void only_the_rows_drawn_show(void)
{
    Capture capture = no_capture();
    capture.beam_x = 0x80;
    fb_init(&fb, 224, 288);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_beam(&fb, &capture, 0);
    CHECK_EQ(lit_in_rows(0, 35), 0);
    set_beam_row(&capture, 10, 1);
    draw_beam(&fb, &capture, 0);
    CHECK_EQ(lit_in_rows(23, 31), 0);
    CHECK(lit_in_rows(32, 32) > 0);
}

int main(void)
{
    RUN(the_beam_column_follows_the_bosses_x);
    RUN(the_beam_palette_cycles_every_4_frames);
    RUN(only_the_rows_drawn_show);
    return check_report();
}
