/* SPDX-License-Identifier: GPL-3.0-only */
#include "collisions.h"
#include "config.h"
#include "render.h"
#include "test/check.h"

/** Large structs: kept out of the stack. */
static Framebuffer fb;
static GameState game;

/** How many pixels of `color` the screen has between rows y0 and y1 (excluded). */
static uint32_t count(uint8_t color, int32_t y0, int32_t y1)
{
    uint32_t n = 0;
    for (int32_t y = y0; y < y1; y++)
        for (int32_t x = 0; x < fb.width; x++)
            n += fb_get(&fb, x, y) == color;
    return n;
}

static void scores_have_four_digits(void)
{
    char text[12];
    format_score(120, text);
    CHECK(text[0] == '0' && text[1] == '1' && text[2] == '2' && text[3] == '0' && !text[4]);
    format_score(12345, text);
    CHECK(text[4] == '5' && !text[5]);
}

static void the_attract_screen_blinks_its_invitation(void)
{
    fb_init(&fb, GAME_WIDTH, GAME_HEIGHT);
    game = game_initial(0);
    render_game(&fb, &game);
    const uint32_t on = count(COLOR_TEXT, ATTRACT_PRESS_Y, ATTRACT_PRESS_Y + 7);
    game.attract_frames = 30;
    render_game(&fb, &game);
    CHECK(on > 0u);
    CHECK_EQ(count(COLOR_TEXT, ATTRACT_PRESS_Y, ATTRACT_PRESS_Y + 7), 0);
}

static void the_playfield_shows_invaders_shields_ground_and_reserve(void)
{
    fb_init(&fb, GAME_WIDTH, GAME_HEIGHT);
    game = game_initial(0);
    game.screen = SCREEN_PLAYING;
    render_game(&fb, &game);
    CHECK(count(COLOR_ALIEN_TOP, 0, GAME_HEIGHT) > 0u);
    CHECK(count(COLOR_ALIEN_BOTTOM, 0, GAME_HEIGHT) > 0u);
    CHECK(count(COLOR_CANNON, SHIELD_Y, SHIELD_Y + 16) > 0u);
    CHECK_EQ(count(COLOR_CANNON, GROUND_Y, GROUND_Y + 1), GAME_WIDTH);
    CHECK_EQ(fb_get(&fb, 24, GROUND_Y + 4 + 4), COLOR_CANNON); /* the first reserve cannon */
}

static void game_over_is_written_in_red(void)
{
    fb_init(&fb, GAME_WIDTH, GAME_HEIGHT);
    game = game_initial(0);
    game.screen = SCREEN_GAME_OVER;
    render_game(&fb, &game);
    CHECK(count(COLOR_UFO, 56, 63) > 0u);
}

int main(void)
{
    RUN(scores_have_four_digits);
    RUN(the_attract_screen_blinks_its_invitation);
    RUN(the_playfield_shows_invaders_shields_ground_and_reserve);
    RUN(game_over_is_written_in_red);
    return check_report();
}
