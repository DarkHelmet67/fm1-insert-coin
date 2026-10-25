/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The mystery ship (insert-coin, ufo.ts). */
#ifndef UFO_H
#define UFO_H

#include <stdint.h>

#include "cartridge/private.h"

/** Height of the mystery ship's lane, just below the score. */
#define UFO_Y 40

/** Frames between two visits: 0x600 = 1536 frames, 25.6 seconds, as in the original. */
#define UFO_INTERVAL 0x600

/** The ship only visits while at least this many invaders are left. */
#define UFO_MIN_ALIENS 8

/** Horizontal limits of the ship's path (the sprite is 16 pixels wide). */
#define UFO_LEFT_X 8
#define UFO_RIGHT_X (224 - 8 - 16)

/** The mystery ship: the countdown to its next visit, and the ship itself when on screen. */
typedef struct {
    uint32_t countdown;
    uint8_t flying;   /**< whether the ship is on screen */
    int8_t direction; /**< 1 to the right, -1 to the left */
    int16_t x;
    uint32_t age; /**< frames since it appeared: drives its warbling sound */
} Ufo;

/** The first visit comes after a full interval. */
GAME_PRIVATE Ufo ufo_initial(void);

/**
 * Points for hitting the ship with the `shots_fired`-th shot. The original steps through a
 * table with every shot and a bug makes it wrap after 15 entries, so the 300 comes back every
 * 15 shots: players know the trick as "the 23rd shot".
 */
GAME_PRIVATE int32_t ufo_score(uint32_t shots_fired);

/** Advances the mystery ship by one frame: fly, or count down to the next visit. */
GAME_PRIVATE Ufo ufo_step(const Ufo *ufo, uint32_t aliens_left, uint32_t shots_fired);

/** The ship shot down: it leaves the screen, the countdown goes on. */
GAME_PRIVATE Ufo ufo_shot_down(const Ufo *ufo);

#endif
