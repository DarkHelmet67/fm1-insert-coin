/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The fighter's two rockets (rockets.ts, `c_1F0F` and `rckt_man`).
 */
#ifndef GALAGA_ROCKETS_H
#define GALAGA_ROCKETS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "objects.h"

/** Sprite palette of rockets. */
#define ROCKET_COLOR 9

/** Number of rockets: two can be in flight at once. */
#define ROCKET_COUNT 2

/**
 * How a rocket travels (`b_92A0[4]`, the "rocket attribute"): its step along x and y every
 * frame. A rocket fired by a fighter that is turning flies along the fighter's direction.
 */
typedef struct {
    int8_t dx;
    int8_t dy;
} RocketPath;

/** The paths of the two rockets. */
typedef struct {
    RocketPath path[ROCKET_COUNT];
} RocketPaths;

/** A rocket's sprite code and path. */
typedef struct {
    int32_t code;
    RocketPath path;
} Rocket;

/**
 * `c_1F0F` and `rckt_man`: the sprite code and the path of a rocket fired by `fighter`. The main
 * axis gets 6 pixels a frame, the other 0, 2, 4 or 6 according to how far the drawing (0
 * horizontal ... 6 upright) is turned: 45 degrees gives 6 and 6. Only a fighter spinning in a
 * capture or a rescue fires sideways.
 */
GAME_PRIVATE Rocket rocket_for(const GameObject *fighter);

/**
 * `c_1F0F`: fires a rocket from the fighter, if one of the two is free. Returns whether a rocket
 * left: the shot sound plays and the shot counter grows.
 */
GAME_PRIVATE int fire_rocket(Objects *objects, RocketPaths *paths, int dual);

/** `rckt_man`: moves both rockets one frame and frees the ones leaving the screen. */
GAME_PRIVATE void move_rockets(Objects *objects, const RocketPaths *paths);

/** No rocket in flight. */
GAME_PRIVATE RocketPaths no_paths(void);

#endif
