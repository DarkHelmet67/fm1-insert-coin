/* SPDX-License-Identifier: GPL-3.0-only */
#include "render.h"

#include "config.h"
#include "test/check.h"

static Framebuffer fb;

static void the_film_tints_the_bricks_by_row(void)
{
    const GameState state = game_initial(0);
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    render_game(&fb, &state);
    CHECK_EQ(fb_get(&fb, 0, 0), COLOR_INK);                        /* the top wall */
    CHECK_EQ(fb_get(&fb, 6, BRICKS_TOP), COLOR_RED_STRIP);         /* first brick */
    CHECK_EQ(fb_get(&fb, 6, BRICKS_TOP + 3), COLOR_BACKGROUND);    /* the gap under a row */
    CHECK_EQ(fb_get(&fb, 6, BRICKS_TOP + 28), COLOR_YELLOW_STRIP); /* last row */
    CHECK_EQ(fb_get(&fb, 100, PADDLE_Y), COLOR_PADDLE_STRIP);      /* the attract paddle */
    CHECK_EQ(fb_get(&fb, 0, 120), COLOR_INK);                      /* the left wall, between */
}

static void the_score_blinks_during_play(void)
{
    GameState state = game_initial(0);
    state.attract = 0;
    state.clock = 8; /* the "off" half */
    fb_init(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
    render_game(&fb, &state);
    CHECK_EQ(fb_get(&fb, DIGITS_LEFT_X, DIGITS_LOWER_Y), COLOR_BACKGROUND);
    state.clock = 0;
    render_game(&fb, &state);
    CHECK_EQ(fb_get(&fb, DIGITS_LEFT_X, DIGITS_LOWER_Y), COLOR_INK);
}

int main(void)
{
    RUN(the_film_tints_the_bricks_by_row);
    RUN(the_score_blinks_during_play);
    return check_report();
}
