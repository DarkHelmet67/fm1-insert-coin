/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One frame of the whole cabinet, from the FM-1's keys to the game, its sound board and its
 * colours: what the launcher runs on the firmware and in the simulator.
 *
 * Like the game, the cabinet is changed in place (see flight.h) instead of being copied every
 * frame on the firmware's stack.
 */
#ifndef GALAGA_APP_H
#define GALAGA_APP_H

#include <stdint.h>

#include "audio/wsg.h"
#include "cartridge/private.h"
#include "game.h"
#include "screen/framebuffer.h"
#include "sound_state.h"

/** The cabinet: the game, the sound board and what the frame loop needs around them. */
typedef struct {
    GameState game;
    SoundState sound;           /**< the sound processor's memory */
    WsgFrame voices;            /**< what the WSG chip plays in the last frame */
    uint32_t sounds;            /**< the sound effects of the last frame: bit 0 the explosion */
    uint16_t colors[FB_COLORS]; /**< the screen's colours, from the colour PROMs */
} App;

/** The cabinet at power-on, with the saved record. */
GAME_PRIVATE void app_start(App *app, int32_t hi_score);

/**
 * The cabinet one frame later.
 * @param app the cabinet, changed in place
 * @param notes the keys held now
 * @param note_presses the keys pressed since the previous frame
 * @param button_presses the buttons pressed since the previous frame
 */
GAME_PRIVATE void app_step(App *app, uint32_t notes, uint32_t note_presses,
                           uint32_t button_presses);

/** The controls of one frame: moving follows the keys held, fire and coin are presses. */
GAME_PRIVATE Controls app_controls(uint32_t notes, uint32_t note_presses, uint32_t button_presses);

#endif
