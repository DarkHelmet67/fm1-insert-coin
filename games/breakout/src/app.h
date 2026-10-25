/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One frame of the whole cabinet, from the FM-1's panel to the game and its sounds: the knob of
 * the cabinet is any of the seven encoders.
 */
#ifndef BREAKOUT_APP_H
#define BREAKOUT_APP_H

#include <stdint.h>

#include "cartridge/cartridge.h"
#include "cartridge/private.h"
#include "controls.h"
#include "game.h"

/** The cabinet: the game and the sounds of its last frame. */
typedef struct {
    GameState game;
    uint32_t sounds; /**< as SOUND_BIT()s */
} App;

/** The cabinet at power-on, with the saved record. */
GAME_PRIVATE App app_initial(int32_t hi_score);

/**
 * The controls of a frame. The encoders' clicks are added up, so any of them turns the knob;
 * the paddle's centre moves KNOB_PIXELS scan lines a click, like the web version's pointer.
 */
GAME_PRIVATE Controls app_controls(const GameState *game, const Panel *panel);

/** The cabinet one frame later. */
GAME_PRIVATE App app_step(const App *app, const Panel *panel);

#endif
