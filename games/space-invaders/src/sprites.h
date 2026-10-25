/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The drawings of Space Invaders, one string per pixel row, copied from the web version
 * (insert-coin, sprites.ts).
 */
#ifndef SPRITES_H
#define SPRITES_H

#include "cartridge/private.h"
#include "sprite/sprite.h"

/** The three kinds of invaders, from the top row (worth most) to the bottom rows. */
typedef enum { ALIEN_SQUID, ALIEN_CRAB, ALIEN_OCTOPUS, ALIEN_KIND_COUNT } AlienKind;

/** The three kinds of invader bombs, in the order they take turns. */
typedef enum { BOMB_ROLLING, BOMB_PLUNGER, BOMB_SQUIGGLY, BOMB_KIND_COUNT } BombKind;

GAME_EXTERN const Sprite CANNON_SPRITE;
GAME_EXTERN const Sprite UFO_SPRITE;
GAME_EXTERN const Sprite SHOT_SPRITE;
GAME_EXTERN const Sprite EXPLOSION_SPRITE;
GAME_EXTERN const Sprite SHOT_EXPLOSION_SPRITE;
GAME_EXTERN const Sprite BOMB_EXPLOSION_SPRITE;
GAME_EXTERN const Sprite SHIELD_SPRITE;

/** An invader in one of its two animation frames. */
GAME_PRIVATE const Sprite *alien_sprite(AlienKind kind, uint32_t frame);

/** A bomb in its animation frame: it changes at every step of its fall. */
GAME_PRIVATE const Sprite *bomb_sprite(BombKind kind, uint32_t steps);

/** The cannon blowing up: two frames that alternate. */
GAME_PRIVATE const Sprite *cannon_explosion_sprite(uint32_t frame);

#endif
