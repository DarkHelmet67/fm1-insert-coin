/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_0857` (bomber-timers.ts): how often each kind of alien leaves the formation, and which of
 * them may drop bombs. The values come from small ROM tables, picked by the stage parameters, by
 * how many aliens are left and by how long the stage has lasted (game timer 2 starts at 120 and
 * ticks twice a second).
 */
#ifndef GALAGA_BOMBER_TIMERS_H
#define GALAGA_BOMBER_TIMERS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "stage_setup.h"

/** What `f_0857` decides (`b_92C0[4-6]` and `b_92C0[8]`). */
typedef struct {
    int32_t reload[3];   /**< launch intervals, in units of 16 frames, of Boss, red and yellow */
    int32_t bomb_bits;   /**< bits allowing a diving alien to drop bombs, one per chance */
    int32_t max_bombers; /**< aliens attacking at once: grows after 30 seconds of the stage */
    int continuous;      /**< few aliens left: they attack nonstop, the formation's pulse stops */
} BomberSettings;

/**
 * The settings for the moment: `alive` aliens, game timer 2 at `timer`; the limit of attackers
 * `max_bombers` grows once and stays.
 */
GAME_PRIVATE BomberSettings bomber_settings(const StageParameters *params, int32_t alive,
                                            int32_t timer, int continuous, int32_t max_bombers);

/**
 * The flag set every frame by the second processor (`b_92A0[$0A]`): fewer aliens than the stage
 * parameter, while the fighter can fire.
 */
GAME_PRIVATE int continuous_bombing(const StageParameters *params, int32_t alive, int fire_enabled);

#endif
