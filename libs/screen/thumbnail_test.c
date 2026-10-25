/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/thumbnail.h"
#include "test/check.h"

static Framebuffer src;
static Framebuffer dst;

static void a_block_takes_its_first_lit_pixel(void)
{
    fb_init(&src, 4, 4);
    CHECK_EQ(thumbnail_block(&src, 0, 0), 0);
    fb_set(&src, 1, 1, 5);
    CHECK_EQ(thumbnail_block(&src, 0, 0), 5);
    fb_set(&src, 1, 0, 3);
    CHECK_EQ(thumbnail_block(&src, 0, 0), 3);
}

static void a_one_pixel_line_survives_the_shrinking(void)
{
    fb_init(&src, 8, 8);
    fb_init(&dst, 8, 8);
    fb_fill_rect(&src, 3, 0, 1, 8, 7); /* an odd column: the left corner would miss it */
    thumbnail_draw(&dst, &src, 2, 2);
    for (int32_t y = 0; y < 4; y++)
        CHECK_EQ(fb_get(&dst, 3, 2 + y), 7);
    CHECK_EQ(fb_get(&dst, 2, 2), 0);
    CHECK_EQ(fb_get(&dst, 3, 6), 0);
}

int main(void)
{
    RUN(a_block_takes_its_first_lit_pixel);
    RUN(a_one_pixel_line_survives_the_shrinking);
    return check_report();
}
