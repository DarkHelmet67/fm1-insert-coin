/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The game state as the canonical string of the reference traces (tools/traces/FORMAT.md):
 * the C port is right when its strings hash like the web version's, frame after frame.
 */
#ifndef TRACE_STATE_H
#define TRACE_STATE_H

#include "cartridge/private.h"
#include "game.h"
#include "trace/text.h"

/** Writes the state string of `state` into `out`, replacing what was there. */
GAME_PRIVATE void trace_state(const GameState *state, Text *out);

/** The screen letter of a trace line: 'A' attract, 'P' playing, 'G' game over. */
GAME_PRIVATE char trace_screen_letter(const GameState *state);

#endif
