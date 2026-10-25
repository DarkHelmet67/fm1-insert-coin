/* SPDX-License-Identifier: GPL-3.0-only */
#include "render.h"

#include "beam.h"
#include "hud.h"
#include "palette.h"
#include "rom.h"
#include "screen/draw.h"
#include "text.h"
#include "tiles.h"

void draw_stars(Framebuffer *fb, const StarField *stars, int32_t frame)
{
    Star visible[STAR_COUNT];
    int32_t count = visible_stars(stars->phase, stars->speed, frame, visible);
    for (int32_t i = 0; i < count; i++)
        fb_set(fb, visible[i].x, visible[i].y, (uint8_t)(STAR_COLOR_BASE + visible[i].color));
}

/** The attract screen: stars, the aliens of the score table, the texts. */
static void render_attract(Framebuffer *fb, const Attract *attract)
{
    draw_stars(fb, &attract->stars, attract->frame);
    draw_objects(fb, &attract->objects);
    draw_messages(fb, &attract->messages);
}

/** A game. */
static void render_playing(Framebuffer *fb, const PlayingState *playing)
{
    draw_stars(fb, &playing->stars, playing->frame);
    draw_objects(fb, &playing->objects);
    draw_beam(fb, &playing->capture, playing->frame);
    draw_messages(fb, &playing->messages);
    draw_badges(fb, playing->stage, playing->badges);
    draw_reserve(fb, playing->reserve);
}

void render_game(Framebuffer *fb, const GameState *state)
{
    fb_clear(fb, COLOR_BACKGROUND);
    if (state->screen == SCREEN_ATTRACT) {
        render_attract(fb, &state->on.attract);
        draw_scores(fb, 0, state->hi_score, 0);
        return;
    }
    render_playing(fb, &state->on.playing);
    draw_scores(fb, state->on.playing.score, state->hi_score,
                ((state->on.playing.frame >> 4) & 1) == 1);
}
