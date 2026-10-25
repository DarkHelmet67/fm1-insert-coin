/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The flight runner of the second processor (flight.ts, `f_08D3`): each of the 12 slots reads its
 * flight program from the ROM and moves its alien one frame. Commands are in flight_commands.h,
 * bombs in flight_bombs.h, the shared data in flight_world.h.
 *
 * **Working copies.** The web version passes its state immutably, building a new copy at every
 * change. Galaga's state is several kilobytes and the FM-1 gives the firmware a stack of about
 * 24 KB, so the Galaga port cannot afford a copy per change: its modules receive a pointer to a
 * working copy and change it in place, and the copy is made once per frame, at the top
 * (game_step()). The modules stay pure in every other way: no globals, no hardware, the same
 * inputs always give the same changes.
 */
#ifndef GALAGA_FLIGHT_H
#define GALAGA_FLIGHT_H

#include "cartridge/private.h"
#include "flight_world.h"

/** One frame of all flight slots, in order. */
GAME_PRIVATE void run_flights(FlightWorld *world, const FlightInputs *inputs);

#endif
