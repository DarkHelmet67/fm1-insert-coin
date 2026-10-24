/* SPDX-License-Identifier: GPL-3.0-only */
#include "sprite/bitmap.h"
#include "test/check.h"

SPRITE_DEFINE(CROSS, 3, ".X.", "XXX", ".X.");
SPRITE_DEFINE(BAD, 3, ".X.", "XX", ".X.");
SPRITE_DEFINE(BAR, 1, "X", "X", "X", "X");

static void a_sprite_knows_its_size_and_pixels(void)
{
    CHECK_EQ(CROSS.width, 3u);
    CHECK_EQ(CROSS.height, 3u);
    CHECK(sprite_lit(&CROSS, 1, 0));
    CHECK(!sprite_lit(&CROSS, 0, 0));
    CHECK(!sprite_lit(&CROSS, 3, 1));
    CHECK(!sprite_lit(&CROSS, -1, 1));
}

static void rows_of_the_wrong_length_are_caught(void)
{
    CHECK(sprite_is_well_formed(&CROSS));
    CHECK(!sprite_is_well_formed(&BAD));
}

static void a_bitmap_has_the_sprites_pixels(void)
{
    const Bitmap cross = bitmap_from_sprite(&CROSS);
    CHECK_EQ(cross.width, 3u);
    CHECK_EQ(bitmap_count(&cross), 5u);
    CHECK(bitmap_lit(&cross, 0, 1));
    CHECK(!bitmap_lit(&cross, 0, 0));
    CHECK_EQ(cross.rows[1], 0xE0000000u);
}

static void bitmaps_collide_only_where_both_are_lit(void)
{
    const Bitmap cross = bitmap_from_sprite(&CROSS);
    const Bitmap bar = bitmap_from_sprite(&BAR);
    const Placed target = {&cross, 10, 10};
    const Placed through_middle = {&bar, 11, 8};
    const Placed in_a_corner = {&bar, 10, 7}; /* only the cross's empty corner (10, 10) */
    const Placed beside = {&bar, 13, 10};     /* touching the right edge */
    CHECK(placed_overlap(&target, &through_middle));
    CHECK(!placed_overlap(&target, &in_a_corner));
    CHECK(!placed_overlap(&target, &beside));
}

static void a_brush_carves_a_hole(void)
{
    const Bitmap cross = bitmap_from_sprite(&CROSS);
    const Bitmap bar = bitmap_from_sprite(&BAR);
    const Placed target = {&cross, 10, 10};
    const Placed brush = {&bar, 11, 9};
    const Bitmap carved = bitmap_erase(&target, &brush);
    CHECK_EQ(bitmap_count(&carved), 2u);
    CHECK(bitmap_lit(&carved, 0, 1));
    CHECK(!bitmap_lit(&carved, 1, 1));
    CHECK_EQ(bitmap_count(&cross), 5u); /* the original is untouched */
}

int main(void)
{
    RUN(a_sprite_knows_its_size_and_pixels);
    RUN(rows_of_the_wrong_length_are_caught);
    RUN(a_bitmap_has_the_sprites_pixels);
    RUN(bitmaps_collide_only_where_both_are_lit);
    RUN(a_brush_carves_a_hole);
    return check_report();
}
