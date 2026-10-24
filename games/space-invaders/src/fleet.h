/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The marching formation (insert-coin, fleet.ts).
 *
 * The original hardware was too slow to redraw 55 invaders every frame, so it moved one invader
 * per frame. A full pass over the formation takes as many frames as there are invaders alive:
 * the famous acceleration is a side effect of this.
 */
#ifndef FLEET_H
#define FLEET_H

#include "aliens.h"

/** Pixels moved sideways at each step; the very last invader moves 3 to the right. */
#define STEP_X 2
#define LAST_ALIEN_STEP_RIGHT 3

/** Pixels moved down when the formation reaches a side of the screen. */
#define STEP_DOWN 8

/** The formation turns around when an invader reaches these horizontal limits. */
#define FLEET_LEFT_LIMIT 8
#define FLEET_RIGHT_LIMIT 216

/** Minimum frames between two notes of the fleet sound. */
#define MIN_BEAT_FRAMES 5

/** The invaders alive and the state of their march. */
typedef struct {
    Alien aliens[FORMATION_SIZE]; /**< in marching order: bottom row first, left to right */
    uint8_t count;                /**< invaders alive */
    uint8_t cursor;               /**< index of the invader that moves in the next frame */
    int8_t direction;             /**< 1 marching right, -1 marching left */
    uint8_t dropping;             /**< whether this pass moves the invaders down */
    uint32_t beat;                /**< number of the fleet sound note to play next */
    uint32_t beat_cooldown;       /**< frames before a note may play again */
} Fleet;

/** The formation at the start of `round`, marching right. */
Fleet fleet_create(uint32_t round);

/** Whether any invader has reached the side of the screen it is marching towards. */
int fleet_touches_edge(const Fleet *fleet);

/** Advances the formation by one frame: moves exactly one invader. */
Fleet fleet_step(const Fleet *fleet);

/** Removes the invader at `index`, keeping the cursor on the one due to move next. */
Fleet fleet_remove(const Fleet *fleet, uint32_t index);

/** The lowest edge reached by the formation, in screen pixels (INT32_MIN when empty). */
int32_t fleet_bottom(const Fleet *fleet);

#endif
