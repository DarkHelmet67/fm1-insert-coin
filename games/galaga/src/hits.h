/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The collisions, checked by the second processor (hits.ts, `hitd_det_rckt`, `hitd_det_fghtr`,
 * `hitd_dspchr`): rockets against the aliens, the fighter against aliens and bombs. A hit does
 * not destroy anything at once: it raises a notification that the main processor turns into an
 * explosion (`f_1DB3`), and adds to the hit counters that the score routine turns into points.
 */
#ifndef GALAGA_HITS_H
#define GALAGA_HITS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "playing.h"
#include "score.h"

/** What the hit checks report besides their changes to the game. */
typedef struct {
    int32_t hit_counts[HIT_COUNTERS]; /**< hits by palette, waiting to become points */
    int32_t quiet_timer;     /**< game timer 1 to set (a diving Boss shot: no bombs), or -1 */
    uint8_t fighters_hit[2]; /**< the fighters hit this frame: $60 and/or $62, in order */
    uint8_t fighters_count;
} HitReport;

/** What the hit checks read. */
typedef struct {
    int dual;
    int rescue_running; /**< a rescued fighter is spinning down: rockets ignore captured ones */
    int waves_entering; /**< the waves still enter: only special enemies and bombs hit */
    ChallengeBonus challenge_bonus;
} HitInputs;

/** An empty report. */
GAME_PRIVATE HitReport no_hit_report(void);

/** One frame of collisions: both rockets, then (when `fighter_checked`) the fighter. */
GAME_PRIVATE void detect_hits(PlayingState *state, const HitInputs *inputs, int fighter_checked,
                              HitReport *report);

/**
 * `f_1DB3`: every hit alien starts its explosion: the notification flag is cleared (what is left
 * is 1, or the score sprite to show), the palette becomes the explosion's.
 */
GAME_PRIVATE void start_explosions(Objects *objects);

/**
 * `hitd_fghtr_hit`: fighter object `id` explodes, a big sprite centered on (x, y), for 15 steps
 * of the object manager.
 */
GAME_PRIVATE void explode_fighter(Objects *objects, int32_t id, int32_t x, int32_t y);

#endif
