/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The launcher's choices, in one place: which controls work the menu, how long PLAY is held to
 * leave a game, the menu's colours and words. The tables are in config.c.
 */
#ifndef LAUNCHER_CONFIG_H
#define LAUNCHER_CONFIG_H

#include <stdint.h>

#include "input/fm1_panel.h"
#include "screen/framebuffer.h"

/**
 * The encoders that choose a game in the menu (Luca, 2026-10-25): the three on the left of the
 * screen. One click is one game; past the last, the first comes back.
 */
#define MENU_ENCODERS                                                                              \
    (1u << FM1_ENCODER_SELECT | 1u << FM1_ENCODER_PRESETS | 1u << FM1_ENCODER_ALGORITHM)

/** PLAY starts the game chosen in the menu; held down in a game, it goes back to the menu. */
#define START_BUTTON FM1_BUTTON_PLAY

/** How long PLAY is held to leave a game: 2 seconds, long enough not to happen by mistake. */
#define LEAVE_FRAMES 120u

/**
 * The menu's colour indexes. The first 112 are the chosen game's own colours, for its picture
 * (Galaga needs 97 of them, with its stars); the menu's own come after them.
 */
enum {
    MENU_COLOR_BACKGROUND = 0,
    MENU_COLOR_TEXT = 112,
    MENU_COLOR_TITLE,
    MENU_COLOR_DIM,
    MENU_COLOR_FRAME,
    MENU_COLOR_COUNT,
};

/** How many of the menu's colours are the chosen game's. */
#define MENU_GAME_COLORS 112u

/** The menu's own colours, at their indexes (the first 112 are taken from the game). */
extern const uint16_t MENU_COLORS[FB_COLORS];

/** The words of the menu. */
#define MENU_TITLE "INSERT COIN"
#define MENU_CHOOSE_TEXT "TURN SELECT TO CHOOSE"
#define MENU_PLAY_TEXT "PRESS PLAY"
#define MENU_BACK_TEXT "HOLD PLAY FOR THE MENU"

#endif
