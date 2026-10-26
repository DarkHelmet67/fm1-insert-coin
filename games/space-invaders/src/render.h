/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Draws the game into a framebuffer at the original resolution, colour indexes of config.h
 * (insert-coin, render.ts, render-playing.ts, render-hud.ts, render-attract.ts).
 */
#ifndef RENDER_H
#define RENDER_H

#include "cartridge/private.h"
#include "game.h"
#include "screen/framebuffer.h"

/** Draws the whole frame from scratch: the framebuffer is the only thing it changes. */
GAME_PRIVATE void render_game(Framebuffer *fb, const GameState *state);

/** The score with four digits, like the original display: 120 becomes "0120". */
GAME_PRIVATE void format_score(int32_t score, char out[12]);

#endif
