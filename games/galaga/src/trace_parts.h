/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The parts of the trace state string (tools/traces/FORMAT.md): numbers in hex as JavaScript's
 * toString(16) writes them, flags as bits, and the bigger pieces of the state (objects, flight
 * slots, attacks, capture...), each as record-galaga.mjs writes it.
 */
#ifndef GALAGA_TRACE_PARTS_H
#define GALAGA_TRACE_PARTS_H

#include <stdint.h>

#include "attacks.h"
#include "capture_state.h"
#include "cartridge/private.h"
#include "clock.h"
#include "formation.h"
#include "messages.h"
#include "objects.h"
#include "phase.h"
#include "stage_setup.h"
#include "star_control.h"
#include "trace/text.h"
#include "wave_launcher.h"

/** A number in hex with no leading zeros, "-" first when negative (`n.toString(16)`). */
GAME_PRIVATE void trace_hex(Text *out, int32_t value);

/** `|name=` before a field (no `|` for the first field of a screen). */
GAME_PRIVATE void trace_field(Text *out, const char *name);

/** The objects: `disposition.counter.code.color.x.y.flags.hit` in hex, separated by `,`. */
GAME_PRIVATE void trace_objects(Text *out, const Objects *objects);

/** The flight slots: `-` for a free one, else 16 hex fields separated by `.`. */
GAME_PRIVATE void trace_slots(Text *out, const Slots *slots);

/** The timers, in decimal, separated by `,`. */
GAME_PRIVATE void trace_timers(Text *out, const GameTimers *timers);

/** The texts on screen: `text@column:row:palette`, separated by `;`. */
GAME_PRIVATE void trace_messages(Text *out, const Messages *messages);

/** The phase of the main program, as the web version names it. */
GAME_PRIVATE void trace_phase(Text *out, const Phase *phase);

/** The formation: offsets / sprite coordinates / direction / bitmap. */
GAME_PRIVATE void trace_formation(Text *out, const Formation *formation);

/** The stage parameters, in decimal, separated by `,`. */
GAME_PRIVATE void trace_params(Text *out, const StageParameters *params);

/** The known looks, by object: `id:code:color:bombs`. */
GAME_PRIVATE void trace_looks(Text *out, const Looks *looks);

/** The wave launcher: tokens / next / wave count / running. */
GAME_PRIVATE void trace_launcher(Text *out, const Launcher *launcher);

/** The attack manager's state, its parts separated by `/`. */
GAME_PRIVATE void trace_attacks(Text *out, const Attacks *attacks);

/** The capture's state, separated by `,`. */
GAME_PRIVATE void trace_capture(Text *out, const Capture *capture);

/** The star control: ship, backwards, target, current, remainder. */
GAME_PRIVATE void trace_star_control(Text *out, const StarControl *control);

#endif
