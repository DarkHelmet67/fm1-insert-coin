/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The whole game: attract screen, play, game over (insert-coin, game.ts). */
#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include "controls.h"
#include "playing.h"

/** Frames the "GAME OVER" message stays on screen before the attract screen returns. */
#define GAME_OVER_FRAMES 300

/** The screen being shown. */
typedef enum { SCREEN_ATTRACT, SCREEN_PLAYING, SCREEN_GAME_OVER } Screen;

/** The whole game state. */
typedef struct {
    Screen screen;
    PlayingState playing;    /**< the game, while playing and on the game over screen */
    int32_t record_to_beat;  /**< the best score when the game started */
    int32_t hi_score;        /**< the best score, which changes during play */
    uint32_t frames_left;    /**< on the game over screen, frames before the attract screen */
    uint32_t attract_frames; /**< frames since the attract screen appeared: animation only */
} GameState;

/** The game when the FM-1 starts: waiting for a coin, with the saved record. */
GameState game_initial(int32_t hi_score);

/** The game one frame later, given the player's controls. */
GameState game_step(const GameState *state, const Controls *controls);

#endif
