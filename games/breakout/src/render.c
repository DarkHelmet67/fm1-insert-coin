/* SPDX-License-Identifier: GPL-3.0-only */
#include "render.h"

#include "config.h"
#include "font/arcade_font.h"
#include "font/font.h"
#include "hud.h"
#include "screen/draw.h"

/** A band of film: its rows and its colour index. */
typedef struct {
    int32_t top;
    int32_t height;
    uint8_t color;
} FilmStrip;

/** Four strips over the bricks, two rows each, and one over the paddle (palette.ts). */
static const FilmStrip FILM[] = {
    {BRICKS_TOP + 0 * 2 * BRICK_HEIGHT, 2 * BRICK_HEIGHT, COLOR_RED_STRIP},
    {BRICKS_TOP + 1 * 2 * BRICK_HEIGHT, 2 * BRICK_HEIGHT, COLOR_ORANGE_STRIP},
    {BRICKS_TOP + 2 * 2 * BRICK_HEIGHT, 2 * BRICK_HEIGHT, COLOR_GREEN_STRIP},
    {BRICKS_TOP + 3 * 2 * BRICK_HEIGHT, 2 * BRICK_HEIGHT, COLOR_YELLOW_STRIP},
    {PADDLE_STRIP_TOP, PADDLE_STRIP_HEIGHT, COLOR_PADDLE_STRIP},
};

void render_film(Framebuffer *fb)
{
    for (uint32_t s = 0; s < sizeof(FILM) / sizeof(FILM[0]); s++)
        for (int32_t y = FILM[s].top; y < FILM[s].top + FILM[s].height; y++)
            for (int32_t x = 0; x < SCREEN_WIDTH; x++)
                if (fb_get(fb, x, y) == COLOR_INK)
                    fb_set(fb, x, y, FILM[s].color);
}

/** The three walls around the playfield. */
static void render_walls(Framebuffer *fb)
{
    fb_fill_rect(fb, 0, 0, SCREEN_WIDTH, TOP_WALL_HEIGHT, COLOR_INK);
    fb_fill_rect(fb, 0, 0, SIDE_WALL_WIDTH, SCREEN_HEIGHT, COLOR_INK);
    fb_fill_rect(fb, RIGHT_WALL_X, 0, SIDE_WALL_WIDTH, SCREEN_HEIGHT, COLOR_INK);
}

/** The bricks still standing. */
static void render_bricks(Framebuffer *fb, const Wall *wall)
{
    for (uint32_t i = 0; i < BRICK_COUNT; i++) {
        if (!wall->standing[i])
            continue;
        const BrickRect r = brick_shape(i);
        fb_fill_rect(fb, r.x, r.y, r.width, r.height, COLOR_INK);
    }
}

/** The ball, if one is in play, on the nearest whole pixel, like a sprite. */
static void render_ball(Framebuffer *fb, const Play *play)
{
    if (play->phase != PHASE_IN_PLAY)
        return;
    fb_fill_rect(fb, ball_pixel(play->ball.x_fifths), ball_pixel(play->ball.y_fifths), BALL_WIDTH,
                 BALL_HEIGHT, COLOR_INK);
}

void render_game(Framebuffer *fb, const GameState *state)
{
    fb_clear(fb, COLOR_BACKGROUND);
    hud_render(fb, state, COLOR_INK);
    render_bricks(fb, &state->round.wall);
    render_walls(fb);
    fb_fill_rect(fb, state->paddle.x, PADDLE_Y, state->paddle.width, PADDLE_HEIGHT, COLOR_INK);
    render_ball(fb, &state->round.play);
    render_film(fb);
    /* after the film, below its strips: the synth's controls, which the cabinet did not need to say
     */
    if (state->attract)
        fb_draw_text_centered(fb, &ARCADE_FONT, ATTRACT_TEXT, ATTRACT_TEXT_Y, COLOR_INK);
}
