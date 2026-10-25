/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The whole game (game.ts): the playfield, the paddle, the colours, the record and a frame
 * counter.
 */
#ifndef BREAKOUT_GAME_H
#define BREAKOUT_GAME_H

#include <stdint.h>

#include "cartridge/private.h"
#include "controls.h"
#include "paddle.h"
#include "play.h"
#include "ticks.h"

/** The game's state. */
typedef struct {
    Round round;
    uint8_t attract; /**< waiting for a player: the ball plays by itself, nothing counts */
    uint8_t mono;    /**< the black and white monitor instead of the coloured film */
    Paddle paddle;
    uint32_t clock;   /**< frames since power-on: when and where a serve appears */
    int32_t hi_score; /**< the best score */
    int32_t
        record_to_beat; /**< the best score when this game started: beating it plays a fanfare */
    Ticks ticks;
} GameState;

/** Power-on: the attract mode, in colour, with a full wall and the saved record. */
GAME_PRIVATE GameState game_initial(int32_t hi_score);

/** The game one frame later. */
GAME_PRIVATE GameState game_step(const GameState *state, const Controls *controls);

#endif
