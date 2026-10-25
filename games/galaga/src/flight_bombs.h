/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The bombs dropped by diving aliens (flight-bombs.ts, `l_0D52`-`l_0DF5`): when they may fall
 * and how they drift towards the fighter.
 */
#ifndef GALAGA_FLIGHT_BOMBS_H
#define GALAGA_FLIGHT_BOMBS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "flight_world.h"

/**
 * Drift rate of a new bomb towards the fighter (`l_0D8D_got_a_bullet`): bit 7 says left, the
 * rest is the horizontal step per vertical one.
 */
GAME_PRIVATE int32_t bomb_drift_rate(int32_t bomb, const GameObject *bomber, int32_t fighter_x);

/**
 * `l_0D52`-`l_0DF5`: every `bomb_interval` frames a diving alien may drop a bomb, if one of its
 * bomb bits allows it, it is in the upper part of the screen and a bomb is free. Changes the
 * working copies of the world and of the slot.
 */
GAME_PRIVATE void try_bombing(FlightWorld *world, FlightSlot *slot, const FlightInputs *inputs);

#endif
