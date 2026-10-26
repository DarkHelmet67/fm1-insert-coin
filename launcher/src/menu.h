/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The menu's screen: the chosen game's picture at half size, moving (its attract screen runs
 * behind it), its name and best score, a dot for every game, and what to press.
 */
#ifndef MENU_H
#define MENU_H

#include <stdint.h>

#include "screen/framebuffer.h"

/** The menu's screen, as large as the display. */
#define MENU_SIZE 240u

/** What the menu shows. */
typedef struct {
    const char *name;  /**< the chosen game */
    int32_t hi_score;  /**< its best score */
    uint32_t selected; /**< its place in the list */
    uint32_t count;    /**< how many games */
    uint32_t frames;   /**< frames since the menu appeared: PLAY blinks */
} MenuView;

/**
 * Draws the whole menu from scratch.
 * @param fb   a MENU_SIZE x MENU_SIZE screen
 * @param view what to show
 * @param game the chosen game's screen, drawn at half size; its colour indexes are below
 *             MENU_GAME_COLORS
 */
void menu_render(Framebuffer *fb, const MenuView *view, const Framebuffer *game);

/**
 * The menu's palette: the chosen game's colours first, for its picture, then the menu's own.
 * @param game_colors the game's FB_COLORS colours
 * @param out         FB_COLORS colours
 */
void menu_colors(const uint16_t game_colors[FB_COLORS], uint16_t out[FB_COLORS]);

/**
 * A score in six digits, with leading zeros, as on the cabinets.
 * @param score at least 0
 * @param out   six characters and the end
 */
void menu_format_score(int32_t score, char out[7]);

#endif
