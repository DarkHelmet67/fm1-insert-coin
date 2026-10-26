/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The clocks of the cabinet (clock.ts: `jp_0513_rst38`, `f_1DD2`). Everything is counted in
 * frames: the frame counter, a slow clock derived from it, and four game timers that tick about
 * twice a second, which the main program uses for its pauses.
 */
#ifndef GALAGA_CLOCK_H
#define GALAGA_CLOCK_H

#include <stdint.h>

#include "cartridge/private.h"

/** The four game timers (`ds4_game_tmrs`), each counting down to 0. */
typedef struct {
    int32_t t[4];
} GameTimers;

/**
 * `ds3_92A0_frame_cts[2]`, the "4 Hz" clock: it steps on frames 0 and 1 of every 32, so it is
 * odd except on one frame in 32, and its bit 1 changes every 32 frames.
 */
GAME_PRIVATE int32_t slow_clock(int32_t frame);

/** `f_1DD2`: the game timers tick on the one frame in 32 where the slow clock is even. */
GAME_PRIVATE int timers_tick(int32_t frame);

/** Timers all at 0. */
GAME_PRIVATE GameTimers no_timers(void);

/** The timers one frame later. */
GAME_PRIVATE GameTimers tick_timers(const GameTimers *timers, int32_t frame);

/** The timers with timer `index` set to `value`. */
GAME_PRIVATE GameTimers set_timer(const GameTimers *timers, int index, int32_t value);

#endif
