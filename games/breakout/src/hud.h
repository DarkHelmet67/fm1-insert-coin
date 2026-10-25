/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The numbers above the bricks (render-hud.ts): player up and score on the left, ball number
 * and the record on the right, in seven-segment digits.
 */
#ifndef BREAKOUT_HUD_H
#define BREAKOUT_HUD_H

#include <stdint.h>

#include "cartridge/private.h"
#include "game.h"
#include "screen/framebuffer.h"

/** Characters of a score: the three digits of the original counter and the end of the string. */
#define HUD_SCORE_CHARS 4u

/** The score as the counter shows it: 7 becomes "007", 1234 becomes "234". */
GAME_PRIVATE void hud_format_score(int32_t score, char out[HUD_SCORE_CHARS]);

/** Whether a blinking score is lit at frame `clock`: on and off in equal halves. */
GAME_PRIVATE int hud_blink_on(uint32_t clock);

/** Draws the numbers in `color`. */
GAME_PRIVATE void hud_render(Framebuffer *fb, const GameState *state, uint8_t color);

#endif
