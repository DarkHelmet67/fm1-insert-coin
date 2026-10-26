/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What the cabinet shows around the play (draw-hud.ts, draw-texts.ts): the scores at the top,
 * the stage badges at the bottom right, the fighters in reserve at the bottom left.
 */
#ifndef GALAGA_HUD_H
#define GALAGA_HUD_H

#include <stdint.h>

#include "cartridge/private.h"
#include "screen/framebuffer.h"

/**
 * A score as the cabinet writes it: right-aligned, at least "00", up to 6 digits. Writes 6
 * characters and the end to `out`.
 */
GAME_PRIVATE char *score_text(int32_t score, char out[7]);

/**
 * The two top lines: "1UP" (blinking during play) and the score on the left, the best score in
 * the middle (`c_093C`, `c_textout_1uphighscore_onetime`).
 */
GAME_PRIVATE void draw_scores(Framebuffer *fb, int32_t score, int32_t hi_score, int blink_off);

/** The stage badges in the bottom right corner, the first `shown` of them. */
GAME_PRIVATE void draw_badges(Framebuffer *fb, int32_t stage, int32_t shown);

/** `draw_resv_ships`: the fighters in reserve at the bottom left. */
GAME_PRIVATE void draw_reserve(Framebuffer *fb, int32_t reserve);

#endif
