/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/framebuffer.h"
#include "test/check.h"

static Framebuffer fb;

static void a_new_screen_is_blank_and_all_changed(void)
{
    fb_init(&fb, 224u, 256u);
    CHECK_EQ(fb.width, 224u);
    CHECK_EQ(fb.height, 256u);
    CHECK_EQ(fb_get(&fb, 100, 100), 0u);
    CHECK(fb_row_dirty(&fb, 0));
    CHECK(fb_row_dirty(&fb, 255));
}

static void the_size_is_capped(void)
{
    fb_init(&fb, 1000u, 1000u);
    CHECK_EQ(fb.width, FB_MAX_WIDTH);
    CHECK_EQ(fb.height, FB_MAX_HEIGHT);
}

static void a_pixel_marks_only_its_row(void)
{
    fb_init(&fb, 224u, 256u);
    fb_clear_dirty(&fb);
    fb_set(&fb, 10, 40, 3u);
    CHECK_EQ(fb_get(&fb, 10, 40), 3u);
    CHECK(fb_row_dirty(&fb, 40));
    CHECK(!fb_row_dirty(&fb, 39));
    CHECK(!fb_row_dirty(&fb, 41));
}

static void the_same_colour_again_changes_nothing(void)
{
    fb_init(&fb, 224u, 256u);
    fb_set(&fb, 10, 40, 3u);
    fb_clear_dirty(&fb);
    fb_set(&fb, 10, 40, 3u);
    CHECK(!fb_row_dirty(&fb, 40));
}

static void outside_the_screen_nothing_happens(void)
{
    fb_init(&fb, 224u, 256u);
    fb_clear_dirty(&fb);
    fb_set(&fb, -1, 0, 1u);
    fb_set(&fb, 224, 0, 1u);
    fb_set(&fb, 0, 256, 1u);
    CHECK_EQ(fb_get(&fb, -1, 0), 0u);
    CHECK(!fb_row_dirty(&fb, 0));
    CHECK(!fb_row_dirty(&fb, 256));
}

static void a_rectangle_is_clipped(void)
{
    fb_init(&fb, 224u, 256u);
    fb_clear_dirty(&fb);
    fb_fill_rect(&fb, 220, 254, 10, 10, 2u);
    CHECK_EQ(fb_get(&fb, 223, 255), 2u);
    CHECK_EQ(fb_get(&fb, 219, 255), 0u);
    CHECK(fb_row_dirty(&fb, 254));
    CHECK(!fb_row_dirty(&fb, 253));
}

static void every_row_can_be_marked(void)
{
    fb_init(&fb, 224u, 256u);
    fb_clear_dirty(&fb);
    fb_mark_all_dirty(&fb);
    CHECK(fb_row_dirty(&fb, 0));
    CHECK(fb_row_dirty(&fb, 131));
}

/** A second screen, for fb_present. */
static Framebuffer back;

static void present_copies_and_marks_only_the_rows_that_differ(void)
{
    fb_init(&fb, 224u, 256u);
    fb_init(&back, 224u, 256u);
    fb_clear_dirty(&fb);
    fb_set(&back, 10, 40, 3);
    fb_set(&back, 11, 200, 5);
    fb_present(&fb, &back);
    CHECK_EQ(fb_get(&fb, 10, 40), 3);
    CHECK_EQ(fb_get(&fb, 11, 200), 5);
    CHECK(fb_row_dirty(&fb, 40));
    CHECK(fb_row_dirty(&fb, 200));
    CHECK(!fb_row_dirty(&fb, 41));
    fb_clear_dirty(&fb);
    fb_present(&fb, &back);
    CHECK(!fb_row_dirty(&fb, 40));
}

int main(void)
{
    RUN(a_new_screen_is_blank_and_all_changed);
    RUN(the_size_is_capped);
    RUN(a_pixel_marks_only_its_row);
    RUN(the_same_colour_again_changes_nothing);
    RUN(outside_the_screen_nothing_happens);
    RUN(a_rectangle_is_clipped);
    RUN(every_row_can_be_marked);
    RUN(present_copies_and_marks_only_the_rows_that_differ);
    return check_report();
}
