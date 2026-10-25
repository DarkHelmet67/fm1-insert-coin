/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One frame of the whole cabinet, from the FM-1's keys to the game, its sounds and its colours:
 * what the firmware (main.c) and the browser simulator (sim/) both run, so the simulator plays
 * exactly the firmware's game.
 */
#ifndef APP_H
#define APP_H

#include <stdint.h>

#include "cartridge/private.h"
#include "game.h"

/** The cabinet: the game and what the frame loop needs around it. */
typedef struct {
    GameState game;
    uint8_t mono;    /**< the black and white monitor instead of the coloured film */
    uint32_t sounds; /**< the sounds of the last frame, as SOUND_BIT()s */
} App;

/** The cabinet at power-on, with the saved record. */
GAME_PRIVATE App app_initial(int32_t hi_score);

/**
 * The cabinet one frame later.
 * @param notes the keys held now
 * @param note_presses the keys pressed since the previous frame
 * @param button_presses the buttons pressed since the previous frame
 */
GAME_PRIVATE App app_step(const App *app, uint32_t notes, uint32_t note_presses,
                          uint32_t button_presses);

/** Whether a new record should be saved now: it went up, and the game is over. */
GAME_PRIVATE int app_record_to_save(const App *app, int32_t saved_hi_score);

#endif
