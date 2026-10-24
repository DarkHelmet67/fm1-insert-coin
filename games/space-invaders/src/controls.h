/* SPDX-License-Identifier: GPL-3.0-only */
/** @file What the player does in one frame, whatever the keys (insert-coin, controls.ts). */
#ifndef CONTROLS_H
#define CONTROLS_H

#include <stdint.h>

/** The buttons of the original cabinet, during one frame. */
typedef struct {
    int8_t direction; /**< -1 left, 1 right, 0 still (also when both are held) */
    uint8_t fire;     /**< fire pressed in this frame (a press, not a hold) */
    uint8_t coin;     /**< a coin inserted in this frame */
} Controls;

#endif
