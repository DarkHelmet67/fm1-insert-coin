/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Draws the whole screen (render.ts): stars at the back, then sprites, then characters on top.
 */
#ifndef GALAGA_RENDER_H
#define GALAGA_RENDER_H

#include "cartridge/private.h"
#include "game.h"
#include "screen/framebuffer.h"

/** Draws the stars of a frame, one pixel each. */
GAME_PRIVATE void draw_stars(Framebuffer *fb, const StarField *stars, int32_t frame);

/** Draws the whole frame from scratch. */
GAME_PRIVATE void render_game(Framebuffer *fb, const GameState *state);

#endif
