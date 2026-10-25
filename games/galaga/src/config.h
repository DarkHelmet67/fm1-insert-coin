/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test (as tuning.config.ts in the web version).
 */
#ifndef GALAGA_CONFIG_H
#define GALAGA_CONFIG_H

#include "stage_setup.h"

/**
 * The difficulty (DIP switches): A is the factory setting, the easiest; B, C and D make the
 * aliens dive and bomb more.
 */
#define GAME_RANK RANK_A

/** Fighters in reserve at the start: 3 fighters in all, the factory setting. */
#define START_RESERVE 2

/**
 * Extra fighters, in tens of thousands of points: the first at 20 000, the second at 70 000,
 * then one every 70 000, the factory setting.
 */
#define EXTRA_LIFE_FIRST 2
#define EXTRA_LIFE_EVERY 7

#endif
