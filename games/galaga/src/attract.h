/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The attract screen (attract.ts, `f_17B2`): while the cabinet waits for a coin, the info screen
 * writes "GALAGA", the score table and the aliens one line a second, then the copyright appears.
 * The original goes on with a training round and a demo game played by the program, which the
 * web version leaves out: the info screen and the copyright repeat instead.
 */
#ifndef GALAGA_ATTRACT_H
#define GALAGA_ATTRACT_H

#include <stdint.h>

#include "cartridge/private.h"
#include "clock.h"
#include "messages.h"
#include "objects.h"
#include "stars.h"

/** Where the attract screen is. */
typedef enum { ATTRACT_INFO, ATTRACT_COPYRIGHT } AttractStep;

/** The attract screen. */
typedef struct {
    int32_t frame;
    StarField stars;
    GameTimers timers;
    uint8_t step;  /**< an AttractStep */
    uint8_t shown; /**< texts of the info screen written so far */
    Messages messages;
    Objects objects;
} Attract;

/** The attract screen as the cabinet is switched on. */
GAME_PRIVATE void new_attract(Attract *attract);

/** One frame of the attract screen: the stars move, the screens follow one another. */
GAME_PRIVATE void update_attract(Attract *attract);

#endif
