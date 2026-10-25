/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What the player does in one frame, whatever the device (controls.ts).
 */
#ifndef BREAKOUT_CONTROLS_H
#define BREAKOUT_CONTROLS_H

#include <stdint.h>

/** The controls of one frame. */
typedef struct {
    int8_t direction;    /**< -1 left, 1 right, 0 still: the arrow keys of the web version */
    uint8_t has_pointer; /**< the pointer moved: pointer_x is where */
    int32_t pointer_x;   /**< where the paddle's centre goes, in scan lines */
    uint8_t serve;       /**< SERVE pressed in this frame; in the attract mode it starts a game */
    uint8_t color_mode;  /**< switch between the black and white monitor and the coloured film */
} Controls;

#endif
