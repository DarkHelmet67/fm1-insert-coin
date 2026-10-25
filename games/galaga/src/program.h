/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The main program of the cabinet (program.ts, `game_ctrl.s`): the sequence of screens of a
 * game, from "PLAYER 1" to the stages, the "READY" after a lost fighter and the restarts. Where
 * it is, is a Phase (phase.h).
 */
#ifndef GALAGA_PROGRAM_H
#define GALAGA_PROGRAM_H

#include "cartridge/private.h"
#include "playing.h"

/** `stg_init_splash`: next stage, its title and its badges; then what `then` says. */
GAME_PRIVATE void start_splash(PlayingState *state, int32_t then);

/** The game at its first frame: "PLAYER 1" and the start theme for 8 timer ticks. */
GAME_PRIVATE void start_intro(PlayingState *state);

/** One frame of the main program. */
GAME_PRIVATE void run_program(PlayingState *state);

#endif
