/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The extra fighters (extra-lives.ts, `gctl_supv_score`): the factory setting of the cabinet
 * gives one at 20 000 points, one at 70 000 and then one every 70 000. The program compares the
 * two leftmost digits of the score with the next threshold after each change of the score.
 */
#ifndef GALAGA_EXTRA_LIVES_H
#define GALAGA_EXTRA_LIVES_H

#include <stdint.h>

#include "cartridge/private.h"
#include "playing.h"

/** The threshold after `bonus`: 2, then 7, then every 7 with the factory setting. */
GAME_PRIVATE int32_t next_bonus_after(int32_t bonus);

/** Adds `points`, and an extra fighter when the score reaches the threshold. */
GAME_PRIVATE void add_points(PlayingState *state, int32_t points);

#endif
