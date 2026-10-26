/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The attract mode (attract.ts), while the cabinet waits for a player.
 */
#ifndef BREAKOUT_ATTRACT_H
#define BREAKOUT_ATTRACT_H

#include <stdint.h>

#include "cartridge/private.h"
#include "play.h"

/**
 * One frame of the attract mode: the ball serves itself and bounces off everything, on a paddle
 * as wide as the row, but the bricks stay and the score stands still: the screen shows the last
 * game until the next one starts.
 */
GAME_PRIVATE Round attract_step(const Round *round, uint32_t clock);

#endif
