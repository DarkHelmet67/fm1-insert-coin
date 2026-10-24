/* SPDX-License-Identifier: GPL-3.0-only */
/** @file From the FM-1's keys and buttons to the cabinet's controls of one frame. */
#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#include "controls.h"
#include "input/keymap.h"

/**
 * The controls of one frame. Moving follows the keys held; fire is a press, as on the web
 * (holding a fire key does not fire again); the coin is a press of `coin_button`.
 * @param map the game keys
 * @param notes the keys held now (fm1_in.notes)
 * @param previous_notes the keys held in the previous frame
 * @param button_presses the buttons pressed since the previous frame (fm1_input_edges)
 * @param coin_button the id of the button that inserts the coin
 */
Controls input_controls(const KeyMap *map, uint32_t notes, uint32_t previous_notes,
                        uint32_t button_presses, uint32_t coin_button);

#endif
