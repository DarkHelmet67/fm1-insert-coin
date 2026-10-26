/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"
#include "palette.h"
#include "playfield.h"
#include "render.h"
#include "test/check.h"

static GameState state;
static Framebuffer fb;

/** Pixels of colour `from` to `to` (included). */
static int32_t count_colors(uint8_t from, uint8_t to)
{
    int32_t n = 0;
    for (int32_t y = 0; y < SCREEN_HEIGHT; y++)
        for (int32_t x = 0; x < SCREEN_WIDTH; x++)
            n += fb_get(&fb, x, y) >= from && fb_get(&fb, x, y) <= to;
    return n;
}

static void the_attract_screen_shows_stars_and_the_scores(void)
{
    create_game(&state, 0);
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    render_game(&fb, &state);
    CHECK(count_colors(STAR_COLOR_BASE, STAR_COLOR_BASE + 63) > 30);
    CHECK(count_colors(SCREEN_COLOR_BASE, SCREEN_COLOR_BASE + 31) > 50); /* the texts */
}

static void a_game_shows_the_reserve_fighters(void)
{
    const Controls coin = {0, 0, 1}, none = {0, 0, 0};
    create_game(&state, 0);
    update_game(&state, &coin);
    for (int i = 0; i < 60; i++)
        update_game(&state, &none);
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    render_game(&fb, &state);
    int32_t bottom = 0;
    for (int32_t y = 34 * 8; y < SCREEN_HEIGHT; y++)
        for (int32_t x = 0; x < 32; x++)
            bottom += fb_get(&fb, x, y) != COLOR_BACKGROUND;
    CHECK(bottom > 20);
}

int main(void)
{
    RUN(the_attract_screen_shows_stars_and_the_scores);
    RUN(a_game_shows_the_reserve_fighters);
    return check_report();
}
