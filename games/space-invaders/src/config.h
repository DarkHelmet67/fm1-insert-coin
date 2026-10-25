/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test (as tuning.config.ts and colors.config.ts in the web
 * version). The tables are in config.c.
 */
#ifndef SPACE_INVADERS_CONFIG_H
#define SPACE_INVADERS_CONFIG_H

#include "cartridge/private.h"
#include "input/fm1_keys.h"
#include "input/keymap.h"
#include "screen/framebuffer.h"
#include "screen/screen_map.h"

/**
 * The controls (Luca, 2026-10-24): the first three labelled black keys move left, the next two
 * fire, the next three move right, so the left hand steers left, the right hand steers right and
 * either thumb can fire.
 */
GAME_EXTERN const KeyMap CONTROLS;

/**
 * The buttons of the cabinet that are not on the keys, as ids of fm1_in.buttons (see
 * platform/docs/API.md): PLAY inserts the coin and starts a game; FX switches between the
 * coloured film and the black and white monitor, as V on the web.
 */
#define COIN_BUTTON 12u /* BTN_PLAY */
#define COLOR_BUTTON 2u /* BTN_FX */

/** The original screen, in pixels (the monitor turned upright). */
#define GAME_WIDTH 224u
#define GAME_HEIGHT 256u

/**
 * The game rows shown on the 240-row display, pixels kept 1:1: the top 8 rows are empty in the
 * original, and between the ground line (row 239) and the reserve cannons (rows 243-250) three
 * empty rows go. 232 + 8 = 240.
 */
GAME_EXTERN const RowRange SHOWN_ROWS[2];

/** The colour indexes of the game's screen. */
enum {
    COLOR_BACKGROUND,
    COLOR_TEXT,
    COLOR_CANNON,
    COLOR_UFO,
    COLOR_SHOTS,
    COLOR_ALIEN_TOP,
    COLOR_ALIEN_MIDDLE,
    COLOR_ALIEN_BOTTOM,
};

/** The colours of the coloured film on the cabinet's glass, as colors.config.ts. */
GAME_EXTERN const uint16_t COLORS[FB_COLORS];

/** Every object in the black and white monitor's single colour. */
GAME_EXTERN const uint16_t MONO_COLORS[FB_COLORS];

/**
 * The lines at the bottom of the attract screen: the web version names the computer's keys
 * (<C> COIN, <V> MONO-COLOR), here the FM-1's. Rows 234-240 of the web version would lose their
 * last row on the display (row 240 is not shown), so the lines sit a little higher.
 */
#define ATTRACT_PRESS_TEXT "PRESS PLAY"
#define ATTRACT_KEYS_TEXT "<OP1-3 FIRE OP4-5 OP6-GLO>"
#define ATTRACT_COLOR_TEXT "<FX> MONO-COLOR"
#define ATTRACT_PRESS_Y 200
#define ATTRACT_KEYS_Y 214
#define ATTRACT_COLOR_Y 226

#endif
