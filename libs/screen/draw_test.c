/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/draw.h"
#include "test/check.h"

SPRITE_DEFINE(CROSS, 3, ".X.", "XXX", ".X.");

static Framebuffer fb;

static void a_sprite_draws_only_its_lit_pixels(void)
{
    fb_init(&fb, 16u, 16u);
    fb_clear(&fb, 1);
    fb_draw_sprite(&fb, &CROSS, 4, 5, 7);
    CHECK_EQ(fb_get(&fb, 5, 5), 7);
    CHECK_EQ(fb_get(&fb, 4, 5), 1);
    CHECK_EQ(fb_get(&fb, 4, 6), 7);
    CHECK_EQ(fb_get(&fb, 6, 7), 1);
}

static void a_bitmap_draws_like_its_sprite(void)
{
    const Bitmap bitmap = bitmap_from_sprite(&CROSS);
    fb_init(&fb, 16u, 16u);
    fb_draw_bitmap(&fb, &bitmap, -1, 0, 2);
    CHECK_EQ(fb_get(&fb, 0, 0), 2);
    CHECK_EQ(fb_get(&fb, 1, 1), 2);
    CHECK_EQ(fb_get(&fb, 1, 0), 0);
}

static void clearing_fills_every_pixel(void)
{
    fb_init(&fb, 16u, 16u);
    fb_clear(&fb, 9);
    CHECK_EQ(fb_get(&fb, 0, 0), 9);
    CHECK_EQ(fb_get(&fb, 15, 15), 9);
}

int main(void)
{
    RUN(a_sprite_draws_only_its_lit_pixels);
    RUN(a_bitmap_draws_like_its_sprite);
    RUN(clearing_fills_every_pixel);
    return check_report();
}
