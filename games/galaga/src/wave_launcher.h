/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_2916` (wave-launcher.ts): sends the attack waves into the screen, one alien at a time, by
 * giving each one a flight slot with the start point and the program of its control byte.
 */
#ifndef GALAGA_WAVE_LAUNCHER_H
#define GALAGA_WAVE_LAUNCHER_H

#include <stdint.h>

#include "attack_waves.h"
#include "cartridge/private.h"
#include "flight_world.h"
#include "stage_setup.h"

/** The launcher's own state. */
typedef struct {
    WaveToken tokens[MAX_TOKENS];
    uint8_t count;
    uint8_t next;       /**< index of the next token (`_p_atkwav_tbl`) */
    uint8_t wave_count; /**< waves started so far (`_b_attkwv_ctr`) */
    uint8_t running;    /**< 0 once all waves are home: the task disables itself */
} Launcher;

/** What the launcher reads from the rest of the game. */
typedef struct {
    int32_t frame;
    int enabled; /**< waves may start (`_b_atk_wv_enbl`): not while a fighter is replaced */
    int32_t flying;
    int challenge;
    int32_t timer; /**< game timer 0, the pause between waves */
    const Looks *looks;
    int32_t entry_bomb_bits;
} LauncherInputs;

/** What the launcher reports besides its changes. */
typedef struct {
    int32_t timer;   /**< game timer 0 to set, or -1 */
    int group_reset; /**< challenging stage: the count of aliens per group restarts */
    int completed;   /**< all waves are home: the attacks start, the sway stops */
} LauncherReport;

/** A running launcher at the start of the tokens of `waves`. */
GAME_PRIVATE void start_launcher(Launcher *launcher, const StageWaves *waves);

/** A launcher with no tokens, not running. */
GAME_PRIVATE Launcher idle_launcher(void);

/** One frame of the launcher. */
GAME_PRIVATE LauncherReport run_launcher(Launcher *launcher, Slots *slots, Objects *objects,
                                         const LauncherInputs *inputs);

#endif
