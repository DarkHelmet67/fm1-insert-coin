/* SPDX-License-Identifier: GPL-3.0-only */
#include "shields.h"
#include "sprites.h"
#include "test/check.h"

static void it_creates_four_shields_evenly_spaced(void)
{
    const Shields shields = shields_create();
    for (int i = 0; i < SHIELD_COUNT; i++) {
        CHECK_EQ(shields.list[i].x, FIRST_SHIELD_X + i * SHIELD_SPACING);
        CHECK_EQ(shields.list[i].y, SHIELD_Y);
    }
}

static void it_finds_a_shield_touched_by_an_object(void)
{
    const Shields shields = shields_create();
    const Bitmap shot = bitmap_from_sprite(&SHOT_SPRITE);
    const Placed on = {&shot, FIRST_SHIELD_X + 10, SHIELD_Y + 2};
    const Placed in_the_arch = {&shot, FIRST_SHIELD_X + 10, SHIELD_Y + 12};
    const Placed beside = {&shot, FIRST_SHIELD_X + 30, SHIELD_Y + 2};
    CHECK(shields_hit(&shields, &on));
    CHECK(!shields_hit(&shields, &in_the_arch));
    CHECK(!shields_hit(&shields, &beside));
}

static void it_carves_a_hole_only_in_the_shield_that_was_hit(void)
{
    const Shields shields = shields_create();
    const Bitmap splash = bitmap_from_sprite(&SHOT_EXPLOSION_SPRITE);
    const Placed brush = {&splash, FIRST_SHIELD_X + 6, SHIELD_Y + 4};
    const Shields damaged = shields_damage(&shields, &brush);
    CHECK(bitmap_count(&damaged.list[0].bitmap) < bitmap_count(&shields.list[0].bitmap));
    for (int i = 1; i < SHIELD_COUNT; i++)
        CHECK_EQ(bitmap_count(&damaged.list[i].bitmap), bitmap_count(&shields.list[i].bitmap));
}

int main(void)
{
    RUN(it_creates_four_shields_evenly_spaced);
    RUN(it_finds_a_shield_touched_by_an_object);
    RUN(it_carves_a_hole_only_in_the_shield_that_was_hit);
    return check_report();
}
