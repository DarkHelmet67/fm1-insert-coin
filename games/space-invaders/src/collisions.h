/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The collision rules (insert-coin, collisions.ts). Every rule takes the state after movement
 * and returns it with one kind of collision resolved; playing.c applies them in order.
 */
#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "cartridge/private.h"
#include "playing.h"

/** Score that earns an extra cannon, once per game. */
#define EXTRA_LIFE_SCORE 1500

/** The playing field's ground line: bombs that reach it explode. */
#define GROUND_Y 239

/** Shot hits invader: the invader explodes and its points are scored. */
GAME_PRIVATE PlayingState shot_hits_alien(const PlayingState *state);

/** Shot hits the mystery ship: its points depend on how many shots have been fired. */
GAME_PRIVATE PlayingState shot_hits_ufo(const PlayingState *state);

/** Shot meets a bomb: both are destroyed. */
GAME_PRIVATE PlayingState shot_hits_bomb(const PlayingState *state);

/** Shot hits a shield: the shot stops and carves a hole. */
GAME_PRIVATE PlayingState shot_hits_shield(const PlayingState *state);

/** A bomb hits the cannon: one life lost, and the game pauses while the cannon explodes. */
GAME_PRIVATE PlayingState bombs_hit_cannon(const PlayingState *state);

/** Bombs hit the shields: each bomb stops and carves a hole. */
GAME_PRIVATE PlayingState bombs_hit_shields(const PlayingState *state);

/** Bombs reach the ground and explode there. */
GAME_PRIVATE PlayingState bombs_hit_ground(const PlayingState *state);

/** Invaders marching over the shields erase the parts they cover. */
GAME_PRIVATE PlayingState aliens_crush_shields(const PlayingState *state);

/** One extra cannon when the score first reaches 1500. */
GAME_PRIVATE PlayingState award_extra_life(const PlayingState *state);

#endif
