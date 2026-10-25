/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The data the flight programs work on (flight-world.ts): the 12 flight slots, the objects they
 * move, the drift of the bombs and the events for the rest of the game.
 */
#ifndef GALAGA_FLIGHT_WORLD_H
#define GALAGA_FLIGHT_WORLD_H

#include <stdint.h>

#include "cartridge/private.h"
#include "flight_slot.h"
#include "formation.h"
#include "objects.h"

/** Number of flight slots: at most 12 aliens fly at once. */
#define SLOT_COUNT 12

/** The flight slots. */
typedef struct {
    FlightSlot list[SLOT_COUNT];
} Slots;

/** What the flight programs read about the rest of the game in this frame. */
typedef struct {
    int32_t frame;          /**< odd and even frames use different speeds */
    int32_t fighter_x;      /**< sprite x of the fighter, 0 when it is not on screen */
    int continuous_bombing; /**< few aliens left: they attack nonstop (`b_92A0[$0A]`) */
    int rescue_running;     /**< a rescued fighter is spinning down to dock (task `f_2000`) */
    int harder_entries;     /**< stage parameters 8 and 9: harder attack paths */
    int harder_attacks;
    int32_t bomb_enable_bits; /**< bits allowing a diving alien to drop bombs (`b_92C0[8]`) */
    int32_t bomb_interval;    /**< frames between tries */
    int bombs_allowed;        /**< the fighter can be bombed: it can fire, is not being captured */
    const Formation *formation;
    int32_t bonus_bee_look; /**< code and palette a bonus bee takes back at home */
} FlightInputs;

/** What a flight program can ask the rest of the game to do. */
typedef enum { EVENT_DIVE_SOUND, EVENT_CAPTURE_DIVE, EVENT_BONUS_BEE_HOME } FlightEventKind;

/** Something a flight program asks the rest of the game to do. */
typedef struct {
    uint8_t kind;     /**< a FlightEventKind */
    uint8_t slot;     /**< capture dive: the slot of the Boss */
    uint8_t target_x; /**< capture dive: where the beam will open */
} FlightEvent;

/** A bomb's horizontal drift: a rate and an accumulator (`b_92B0`). */
typedef struct {
    uint8_t rate;
    uint8_t accumulator;
} BombDrift;

/** The most events in a frame: one per slot and command is more than enough. */
#define MAX_EVENTS 24u

/** The events of one frame of flight. */
typedef struct {
    FlightEvent list[MAX_EVENTS];
    uint8_t count;
} FlightEvents;

/**
 * Everything the flight runner reads and changes: parts of the game state, by pointer, so that
 * the runner changes the game's own working copy (see flight.h).
 */
typedef struct {
    Slots *slots;
    Objects *objects;
    BombDrift *bomb_drift; /**< drift of the 8 bombs, in the order of their objects */
    FlightEvents *events;
} FlightWorld;

/** One more event, if there is room. */
GAME_PRIVATE void flight_emit(FlightWorld *world, FlightEventKind kind, int32_t slot,
                              int32_t target_x);

#endif
