/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What the launcher shows and when it changes: the menu, where the encoders choose a game, or a
 * game, until PLAY is held down. It decides only; console.c carries the decisions out.
 */
#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <stdint.h>

#include "cartridge/cartridge.h"

/** The menu, or a game being played. */
typedef enum { LAUNCHER_MENU, LAUNCHER_GAME } LauncherScreen;

/** The launcher's state. */
typedef struct {
    uint8_t screen;       /**< a LauncherScreen */
    uint32_t selected;    /**< the game chosen, 0 to count - 1 */
    uint32_t count;       /**< how many games there are */
    uint32_t held_frames; /**< in a game, frames PLAY has been held down */
    uint32_t menu_frames; /**< frames since the menu appeared: for the blinking */
} Launcher;

/**
 * The menu at power-on.
 * @param count    how many games (at least 1)
 * @param selected the game chosen at first
 */
Launcher launcher_initial(uint32_t count, uint32_t selected);

/** The clicks of the menu's encoders (MENU_ENCODERS) added up: + is the next game. */
int32_t launcher_menu_turns(const Panel *panel);

/** The game `turns` places after `selected`, round the list both ways. */
uint32_t launcher_wrap(uint32_t selected, int32_t turns, uint32_t count);

/** The launcher one frame later. */
Launcher launcher_step(const Launcher *launcher, const Panel *panel);

#endif
