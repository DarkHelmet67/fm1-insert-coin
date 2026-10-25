/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The game's state as the canonical string of the reference traces (tools/traces/FORMAT.md),
 * the same string tools/traces/record-galaga.mjs writes from the web version. The parts of the
 * string are written by trace_parts.h.
 */
#ifndef GALAGA_TRACE_STATE_H
#define GALAGA_TRACE_STATE_H

#include "cartridge/private.h"
#include "game.h"
#include "trace/text.h"

/** Writes the state string of `state` into `out` (cleared first). */
GAME_PRIVATE void trace_state(const GameState *state, Text *out);

/** The screen letter of a trace line: 'A' attract, 'P' playing. */
GAME_PRIVATE char trace_screen_letter(const GameState *state);

#endif
