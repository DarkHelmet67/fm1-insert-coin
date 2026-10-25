/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The whole game (game.ts): the attract screen or a game in progress, and the record that
 * survives from one game to the next.
 */
#ifndef GALAGA_GAME_H
#define GALAGA_GAME_H

#include <stdint.h>

#include "attract.h"
#include "cartridge/private.h"
#include "controls.h"
#include "playing.h"

/** The screen being shown. */
typedef enum { SCREEN_ATTRACT, SCREEN_PLAYING } Screen;

/** The whole game state: only one of the two screens is alive at a time. */
typedef struct {
    uint8_t screen; /**< a Screen */
    int32_t hi_score;
    union {
        Attract attract;
        PlayingState playing;
    } on;
} GameState;

/** The cabinet waiting for a coin, with `hi_score` as the record to beat. */
GAME_PRIVATE void create_game(GameState *state, int32_t hi_score);

/**
 * A new game, at the "PLAYER 1" screen. The stage counter goes up by one before each stage, so
 * it starts one below the first stage.
 */
GAME_PRIVATE void new_game(PlayingState *playing);

/**
 * The game one frame later, in place: the state is the frame's working copy (see flight.h). A
 * coin on the attract screen starts a game.
 */
GAME_PRIVATE void update_game(GameState *state, const Controls *controls);

#endif
