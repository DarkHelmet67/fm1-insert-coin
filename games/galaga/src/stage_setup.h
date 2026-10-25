/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What changes from one stage to the next (stage-setup.ts): the look of the aliens (`c_2896`)
 * and the ten difficulty parameters of the stage (`c_2C00`), from tables of the main ROM.
 */
#ifndef GALAGA_STAGE_SETUP_H
#define GALAGA_STAGE_SETUP_H

#include <stdint.h>

#include "cartridge/private.h"

/**
 * The difficulty set with the DIP switches, as the index the tables use: they are stored in the
 * order B, C, D, A. A is the factory setting.
 */
typedef enum { RANK_B, RANK_C, RANK_D, RANK_A } Rank;

/** How an alien looks when it enters: sprite code (multiple of 8), palette, may drop bombs. */
typedef struct {
    uint8_t code;
    uint8_t color;
    uint8_t bombs;
} AlienLook;

/** The look of every object, $00-$7E; `known` is 0 where the stage set none. */
typedef struct {
    AlienLook look[64];
    uint8_t known[64];
} Looks;

/** The bonus of a challenging stage for each group of 8 aliens shot down. */
typedef struct {
    int32_t tens; /**< points in hundreds (`ds_bug_collsn_hit_mult[$0F]`): 10 is 1000 points */
    int32_t
        score_code; /**< the score sprite shown on the last alien of the group, with the hit flag */
} ChallengeBonus;

/** The ten parameters of a stage (`ds_new_stage_parms`), named after their use. */
typedef struct {
    uint8_t bomb_enable; /**< index into the "bomb enable" table (0) */
    uint8_t boss_timer;  /**< launch timer tables of Boss, red and yellow bombers (1-3) */
    uint8_t red_timer;
    uint8_t yellow_timer;
    uint8_t max_bombers;        /**< aliens attacking at once (4) */
    uint8_t max_bombers_later;  /**< ... after the first 30 seconds (5) */
    uint8_t beam_speed;         /**< frames between steps of the tractor beam (6) */
    uint8_t continuous_bombing; /**< with fewer aliens than this left, they attack nonstop (7) */
    uint8_t harder_entries;     /**< harder entry paths (8) */
    uint8_t harder_attacks;     /**< harder attack paths (9) */
    uint8_t clone_attack; /**< aliens left when the triple "clone" attack may start; 0 at first */
} StageParameters;

/** Whether `stage` (1, 2, ...) is a challenging stage: 3, 7, 11 ... */
GAME_PRIVATE int is_challenge_stage(int32_t stage);

/** No looks at all. */
GAME_PRIVATE Looks no_looks(void);

/**
 * `c_2896`: the look of objects $08-$5E at the start of `stage`. On challenging stages the bees
 * and butterflies are replaced by the special aliens of that stage, every four stages a new kind.
 */
GAME_PRIVATE Looks stage_looks(int32_t stage);

/** `l_28B5`: the group bonus grows every 8 stages, up to the fourth level from stage 32. */
GAME_PRIVATE ChallengeBonus challenge_bonus(int32_t stage);

/** `c_2C00`: the parameters of `stage` at difficulty `rank`, one per nibble of 5 bytes. */
GAME_PRIVATE StageParameters stage_parameters(int32_t stage, int32_t rank);

/** `c_2C00`: star speed target of the stage, in 1/64 pixel per frame. */
GAME_PRIVATE int32_t star_speed_target(int32_t stage);

#endif
