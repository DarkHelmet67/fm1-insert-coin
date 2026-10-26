/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The two scripted sequences of the main program (endings.ts): the results of a challenging
 * stage (`gctl_chllng_stg_end`) and the end of the game (`gctl_plyr_terminate`). The original
 * runs them as straight code with pauses (`c_tdelay_3`); the port keeps a table of lines, each
 * an action followed by a wait.
 */
#ifndef GALAGA_ENDINGS_H
#define GALAGA_ENDINGS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "phase.h"
#include "playing.h"

/** What a line waits for before the next one runs. */
typedef enum {
    WAIT_PAUSE,   /**< `c_tdelay_3`: game timer 3 set to 3 ticks, about a second and a half */
    WAIT_BLINK,   /**< the next frame whose number is a multiple of 16 ("PERFECT !" blinks) */
    WAIT_BEAM,    /**< the tractor beam has finished (`l_0509`) */
    WAIT_RESULTS, /**< game timer 2 set to 14 ticks: the results stay about 7 seconds */
} Wait;

/** Number of lines of script `name`. */
GAME_PRIVATE int32_t script_length(int32_t name);

/** What line `line` of script `name` waits for. */
GAME_PRIVATE Wait script_wait(int32_t name, int32_t line);

/** Runs the action of line `line` of script `name`. */
GAME_PRIVATE void run_script_line(PlayingState *state, int32_t name, int32_t line);

/** Sets up `wait`, right after a line has run. */
GAME_PRIVATE void start_wait(PlayingState *state, Wait wait);

/** Whether `wait` is over in this frame. */
GAME_PRIVATE int wait_over(const PlayingState *state, Wait wait);

/**
 * `c_0A72_puts_hitmiss_ratio`: hits per 100 shots with one decimal, rounded; no leading zeros,
 * so a ratio under 1% starts with the dot. Writes it to `out` (12 characters are enough).
 */
GAME_PRIVATE char *hit_ratio(int32_t hits, int32_t shots, char *out);

#endif
