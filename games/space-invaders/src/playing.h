/* SPDX-License-Identifier: GPL-3.0-only */
/** @file One game being played (insert-coin, playing.ts). */
#ifndef PLAYING_H
#define PLAYING_H

#include <stdint.h>

#include "bombs.h"
#include "cannon.h"
#include "controls.h"
#include "effects.h"
#include "fleet.h"
#include "shields.h"
#include "ufo.h"

/** Cannons at the start of a game. */
#define STARTING_LIVES 3

/** Everything on screen during a game. */
typedef struct {
    uint32_t round;
    int32_t score;
    int32_t lives; /**< cannons left, including the one in play */
    uint8_t extra_life_awarded;
    uint32_t shots_fired;      /**< decides the mystery ship's points and direction */
    int32_t cannon_x;          /**< left edge of the cannon */
    uint32_t cannon_explosion; /**< frames left in the cannon's explosion, 0 while it is alive */
    Shot shot;
    Fleet fleet;
    Bombs bombs;
    Shields shields;
    Ufo ufo;
    Effects effects;
} PlayingState;

/** A new game: three cannons, the formation of `round`, four intact shields. */
PlayingState playing_initial(uint32_t round);

/** Whether the game has ended: no cannons left after the last explosion, or the invaders landed. */
int playing_is_over(const PlayingState *state);

/**
 * The game one frame later. Like the original, the logic advances in whole frames (1/60 s):
 * the formation moves one invader per frame and every bomb moves once every 3 frames.
 */
PlayingState playing_step(const PlayingState *state, const Controls *controls);

#endif
