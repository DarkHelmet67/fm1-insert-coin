/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What the player does during one frame (controls.ts), whatever the keys used: the cabinet's
 * stick, fire button and coin.
 */
#ifndef GALAGA_CONTROLS_H
#define GALAGA_CONTROLS_H

#include <stdint.h>

/** The controls of one frame. */
typedef struct {
    int8_t direction; /**< -1 left, 1 right, 0 none; both held move right, as on the cabinet */
    uint8_t fire;     /**< the fire button went down in this frame: one press, one rocket */
    uint8_t coin;     /**< a coin, which also starts the game */
} Controls;

#endif
