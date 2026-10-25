/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_1B65`, the attack manager (attacks.ts). Every 16 frames three countdowns (Boss, red
 * butterfly, yellow bee) step down; when one runs out an alien of that kind leaves the formation
 * and dives along its attack program, unless too many are already flying. A Boss takes up to two
 * butterflies of the row below as escorts; their launches wait in a small queue and leave one
 * per frame.
 */
#ifndef GALAGA_ATTACKS_H
#define GALAGA_ATTACKS_H

#include <stdint.h>

#include "bomber_timers.h"
#include "capture_state.h"
#include "cartridge/private.h"
#include "flight_world.h"

/** Flight programs of the second processor's ROM used by the attacks. */
enum {
    PROGRAM_BEE = 0x034f,           /**< `db_flv_atk_yllw`: bee dive */
    PROGRAM_BUTTERFLY = 0x03a9,     /**< `db_flv_atk_red`: butterfly dive */
    PROGRAM_BOSS = 0x0411,          /**< `db_flv_0411`: Boss dive */
    PROGRAM_BOSS_TRAINING = 0x00f1, /**< `db_flv_00f1`: Boss dive when aliens are not "enabled" */
    PROGRAM_ROGUE_FIGHTER = 0x0444, /**< `db_fltv_rogefgter`: a captured fighter diving alone */
    PROGRAM_CAPTURE = 0x0454,       /**< `db_0454`: a Boss diving to capture the fighter */
    PROGRAM_CAPTURE_HOME = 0x046b,  /**< `db_flv_cboss`: the Boss going home with the fighter */
};

/** A launch waiting in the queue (`bmbr_boss_pool`). */
typedef struct {
    uint8_t object;
    uint8_t mirrored;
    uint16_t program;
} QueuedLaunch;

/** The score a Boss is worth when shot while diving, set when it leaves. */
typedef struct {
    uint8_t hundreds; /**< points added in hundreds (`ds_bug_collsn_hit_mult[$0F]`) */
    uint8_t code;     /**< the score sprite shown, with the hit flag */
} BossScore;

/** The longest queue: a Boss, two escorts and its captured fighter. */
#define QUEUE_MAX 4

/** The attack manager's state. */
typedef struct {
    uint8_t countdown[3]; /**< countdowns of Boss, red and yellow bombers (`b_92C0[0-2]`) */
    QueuedLaunch queue[QUEUE_MAX];
    uint8_t queue_length;
    /** Counts Boss launches while no capture is going on: one in two captures. */
    int32_t boss_toggle;
    BossScore boss_scores[4]; /**< score of each Boss when shot diving, by place ($30-$36) */
    uint8_t bonus_bee;        /**< the alien chosen as bonus bee, not launched (`_b_bbee_obj`) */
    int32_t bonus_bee_timer;  /**< frames before it leaves, up to 0 from $C0 (`_b_bbee_tmr`) */
    uint8_t bonus_bee_look;   /**< its own look, code and palette, taken back at home */
    uint8_t bonus_bee_color;  /**< the palette it flashes to, 4, 5 or 6 by stage */
} Attacks;

/** What the manager changes: parts of the game's working copy (see flight.h). */
typedef struct {
    Attacks *attacks;
    Objects *objects;
    Slots *slots;
    Capture *capture; /**< only the choice of a capturing Boss reads and changes it */
    int dived;        /**< set when an alien left the formation: the dive sound */
} AttackWorld;

/** What the manager reads. */
typedef struct {
    int32_t frame;
    BomberSettings settings;
    int32_t flying;
    int enemies_enabled;
    int fire_enabled;
    int rescue_running;
} AttackInputs;

/** The manager at the start of a stage. */
GAME_PRIVATE Attacks new_attacks(void);

/** The first free flight slot, or -1. */
GAME_PRIVATE int32_t first_free_slot(const Slots *slots);

/**
 * `j_108A`: an alien leaves the formation. It takes a free flight slot, starting where its
 * sprite is, pointing up.
 */
GAME_PRIVATE void launch_diver(AttackWorld *world, const QueuedLaunch *launch, int32_t bomb_bits);

/** One frame of the attack manager. */
GAME_PRIVATE void run_attacks(AttackWorld *world, const AttackInputs *inputs);

#endif
