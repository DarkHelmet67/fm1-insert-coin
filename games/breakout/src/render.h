/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The screen (render.ts, render-film.ts): as on the cabinet, everything is drawn in the
 * monitor's one colour, then the strips of film glued on the glass tint the bands they cover.
 */
#ifndef BREAKOUT_RENDER_H
#define BREAKOUT_RENDER_H

#include "cartridge/private.h"
#include "game.h"
#include "screen/framebuffer.h"

/**
 * Lays the film: in the rows of each strip, the ink takes the strip's colour index. The black
 * and white monitor gives those indexes white (MONO_COLORS), so the film is always laid.
 */
GAME_PRIVATE void render_film(Framebuffer *fb);

/** Draws the whole frame, from scratch. */
GAME_PRIVATE void render_game(Framebuffer *fb, const GameState *state);

#endif
