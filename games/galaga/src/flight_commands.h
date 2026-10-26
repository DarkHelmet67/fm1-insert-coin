/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The commands $EF-$FF of the flight programs (flight-commands.ts, `d_0920_jp_tbl`): jumps,
 * aiming at the fighter, heading home, the capture dive, the bonus bee's split.
 *
 * Like the rest of the flight runner, these functions change a working copy: the world and the
 * slot being processed, which the runner writes back into the world when the slot is done (see
 * flight.h for why the Galaga port works on copies).
 */
#ifndef GALAGA_FLIGHT_COMMANDS_H
#define GALAGA_FLIGHT_COMMANDS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "flight_world.h"

/** How the processing of a slot continues after a command. */
typedef enum {
    NEXT_READ, /**< read the next instruction at `slot->pointer` (`j_090E_flite_path_init`) */
    NEXT_FLY,  /**< keep the current movement and fly this frame (`l_0BFF`) */
    NEXT_WAIT, /**< done for this frame; read the next instruction on the next one (`l_0B8B`) */
    NEXT_STOP, /**< the slot is free and the alien gone (`case_0E49`) */
} Next;

/** Command $FF: the alien leaves the game and slot `index` is freed. */
GAME_PRIVATE Next stop_flying(FlightWorld *world, int32_t index, const FlightSlot *slot);

/**
 * Runs command `command` (the complement of the program byte, 0 for $FF) for slot `index`,
 * whose working copy is `slot`.
 */
GAME_PRIVATE Next run_command(int32_t command, FlightWorld *world, int32_t index, FlightSlot *slot,
                              const FlightInputs *inputs);

#endif
