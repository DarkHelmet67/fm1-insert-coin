/* SPDX-License-Identifier: GPL-3.0-only */
#include "input.h"

Controls input_controls(const KeyMap *map, uint32_t notes, uint32_t note_presses,
                        uint32_t button_presses, uint32_t coin_button)
{
    const uint32_t held = keymap_actions(map, notes);
    const uint32_t pressed = keymap_actions(map, note_presses);
    Controls controls;
    controls.direction = (int8_t)((held & ACTION_LEFT) ? -1 : (held & ACTION_RIGHT) ? 1 : 0);
    controls.fire = (pressed & ACTION_FIRE) != 0u;
    controls.coin = (button_presses >> coin_button & 1u) != 0u;
    return controls;
}
