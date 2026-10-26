/* SPDX-License-Identifier: GPL-3.0-only */
#include "fighter.h"
#include "objects.h"
#include "palette.h"
#include "playfield.h"
#include "screen/draw.h"
#include "test/check.h"
#include "tiles.h"

static Framebuffer fb;

/** Pixels of the square (left, top, size) that are not background. */
static int32_t lit_in(int32_t left, int32_t top, int32_t size)
{
    int32_t n = 0;
    for (int32_t y = top; y < top + size; y++)
        for (int32_t x = left; x < left + size; x++)
            n += fb_get(&fb, x, y) != COLOR_BACKGROUND;
    return n;
}

static void the_upright_fighter_has_pixels_of_its_three_colours(void)
{
    int32_t used[4] = {0, 0, 0, 0};
    for (int32_t y = 0; y < 16; y++)
        for (int32_t x = 0; x < 16; x++)
            used[sprite_pixel(FIGHTER_UPRIGHT, x, y)]++;
    CHECK(used[0] > 0); /* see-through around it */
    CHECK(used[1] + used[2] + used[3] > 30);
}

static void an_object_is_drawn_at_its_sprite_position_minus_the_offsets(void)
{
    Objects objects = empty_objects();
    place_fighter(&objects);
    const GameObject *fighter = object_at(&objects, FIGHTER);
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_objects(&fb, &objects);
    const int32_t left = fighter->x - SPRITE_OFFSET_X;
    const int32_t top = fighter->y - SPRITE_OFFSET_Y;
    CHECK(lit_in(left, top, 16) > 30);
    CHECK_EQ(lit_in(left - 16, top, 16), 0);
}

static void a_hidden_object_draws_nothing(void)
{
    Objects objects = empty_objects();
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_objects(&fb, &objects);
    CHECK_EQ(lit_in(0, 0, SCREEN_WIDTH), 0);
}

static void see_through_character_pixels_leave_the_background(void)
{
    const TileColors none = {{SEE_THROUGH_INDEX, SEE_THROUGH_INDEX, SEE_THROUGH_INDEX}};
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_char(&fb, 0x0a, 3, 3, &none);
    CHECK_EQ(lit_in(24, 24, 8), 0);
}

int main(void)
{
    RUN(the_upright_fighter_has_pixels_of_its_three_colours);
    RUN(an_object_is_drawn_at_its_sprite_position_minus_the_offsets);
    RUN(a_hidden_object_draws_nothing);
    RUN(see_through_character_pixels_leave_the_background);
    return check_report();
}
