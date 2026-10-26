/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The brick ticks (ticks.ts): the circuit adds a brick's points one at a time and ticks at each
 * one, so the ticks trail behind the ball.
 */
#ifndef BREAKOUT_TICKS_H
#define BREAKOUT_TICKS_H

#include <stdint.h>

#include "cartridge/private.h"

/** The points owed, the ticks played, the frames before the next one may play. */
typedef struct {
    int32_t owed;
    uint32_t played;
    uint32_t wait;
} Ticks;

/** No ticks owed or played: the start of a game. */
GAME_PRIVATE Ticks ticks_none(void);

/** Adds the points scored in this frame and plays the next tick when it is due. */
GAME_PRIVATE Ticks ticks_step(const Ticks *ticks, int32_t points);

#endif
