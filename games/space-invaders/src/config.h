/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test (as tuning.config.ts in the web version).
 */
#ifndef SPACE_INVADERS_CONFIG_H
#define SPACE_INVADERS_CONFIG_H

#include "input/fm1_keys.h"
#include "input/keymap.h"
#include "screen/framebuffer.h"
#include "screen/screen_map.h"

/**
 * The controls (Luca, 2026-10-24): the first three labelled black keys move left, the next two
 * fire, the next three move right, so the left hand steers left, the right hand steers right and
 * either thumb can fire.
 */
static const KeyMap CONTROLS = {
    .left = FM1_KEY_BIT(FM1_KEY_OP1) | FM1_KEY_BIT(FM1_KEY_OP2) | FM1_KEY_BIT(FM1_KEY_OP3),
    .fire = FM1_KEY_BIT(FM1_KEY_OP4) | FM1_KEY_BIT(FM1_KEY_OP5),
    .right = FM1_KEY_BIT(FM1_KEY_OP6) | FM1_KEY_BIT(FM1_KEY_PIT) | FM1_KEY_BIT(FM1_KEY_GLO),
};

/** The original screen, in pixels (the monitor turned upright). */
#define GAME_WIDTH 224u
#define GAME_HEIGHT 256u

/**
 * The game rows shown on the 240-row display, pixels kept 1:1: the top 8 rows are empty in the
 * original, and between the ground line (row 239) and the reserve cannons (rows 243-250) three
 * empty rows go. 232 + 8 = 240.
 */
static const RowRange SHOWN_ROWS[] = {{8u, 239u}, {243u, 250u}};

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

/** The colours of the coloured film on the cabinet's glass, as colors.config.ts of the web version.
 */
static const uint16_t COLORS[FB_COLORS] = {
    [COLOR_BACKGROUND] = RGB565(0x00, 0x00, 0x00),
    [COLOR_TEXT] = RGB565(0xFF, 0xFF, 0xFF),
    [COLOR_CANNON] = RGB565(0x30, 0xFF, 0x30),
    [COLOR_UFO] = RGB565(0xFF, 0x30, 0x30),
    [COLOR_SHOTS] = RGB565(0xFF, 0xFF, 0xFF),
    [COLOR_ALIEN_TOP] = RGB565(0x40, 0xC8, 0xFF),
    [COLOR_ALIEN_MIDDLE] = RGB565(0x30, 0xFF, 0x30),
    [COLOR_ALIEN_BOTTOM] = RGB565(0xC0, 0x50, 0xFF),
};

#endif
