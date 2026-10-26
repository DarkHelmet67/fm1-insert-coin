/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_0ECA` and `c_0F0B` (score.ts): the hit counters become points. Each palette has its value in
 * the table `d_scoreman_inc_lut`, two BCD digits for tens and hundreds; counter 15 carries the
 * bonuses, in hundreds.
 */
#ifndef GALAGA_SCORE_H
#define GALAGA_SCORE_H

#include <stdint.h>

#include "cartridge/private.h"

/** Counters of hits by palette (`ds_bug_collsn_hit_mult`). */
#define HIT_COUNTERS 16

/** Points of one hit of palette `color`: the table is read backwards. */
GAME_PRIVATE int32_t points_of(int32_t color);

/** The points of all the counted hits. */
GAME_PRIVATE int32_t hit_points(const int32_t counts[HIT_COUNTERS]);

#endif
