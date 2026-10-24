/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The player's laser cannon and its shot (insert-coin, cannon.ts and shot.ts). */
#ifndef CANNON_H
#define CANNON_H

#include <stdint.h>

/** Cannon width in pixels, as in the original sprite. */
#define CANNON_WIDTH 13

/** Leftmost and rightmost positions reachable by the cannon. */
#define CANNON_MIN_X 16
#define CANNON_MAX_X (224 - 16 - CANNON_WIDTH)

/** Vertical position of the cannon's top edge, in screen pixels. */
#define CANNON_Y 216

/** Frames the game pauses while the cannon explodes (chosen by eye in the web version). */
#define CANNON_EXPLOSION_FRAMES 90

/** The shot climbs 4 pixels per frame, less than an invader is tall: it cannot jump over one. */
#define SHOT_SPEED 4

/** The shot disappears when it goes above this height, just below the score. */
#define SHOT_TOP_LIMIT 32

/** The cannon's laser shot: only one can fly at a time, as in the original. */
typedef struct {
    uint8_t active; /**< 0 when the cannon is ready to fire */
    int16_t x;
    int16_t y;
} Shot;

/** No shot on screen. */
#define NO_SHOT ((Shot){0, 0, 0})

/** The cannon's left edge after one frame moving `direction` (-1, 0, 1), within the edges. */
int32_t cannon_move(int32_t x, int32_t direction);

/** Horizontal centre of the cannon: where the shot leaves and what the rolling bomb aims at. */
int32_t cannon_center(int32_t x);

/** A new shot leaving the tip of the cannon. */
Shot shot_fire(int32_t cannon_x);

/** The shot one frame later: gone once it has left the playfield. */
Shot shot_move(const Shot *shot);

#endif
