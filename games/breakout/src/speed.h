/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The ball's speed, derived at every frame from a few flags as the circuit did (speed.ts).
 */
#ifndef BREAKOUT_SPEED_H
#define BREAKOUT_SPEED_H

#include <stdint.h>

#include "cartridge/private.h"

/** How far the ball moves in one frame, in fifths: steps down or up, scan lines to the side. */
typedef struct {
    int32_t vertical;
    int32_t sideways;
} Speed;

/**
 * The ball's speed for its state.
 * @param hits  hits counted since the serve
 * @param fast  set by an orange or red brick: the fastest speed until the next serve
 * @param outer the ball last touched an outer segment of the paddle (a flatter angle)
 */
GAME_PRIVATE Speed speed_for(uint32_t hits, int fast, int outer);

#endif
