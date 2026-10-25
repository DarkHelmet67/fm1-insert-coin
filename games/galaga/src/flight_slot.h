/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One of the 12 flight slots of the second processor (flight-slot.ts, `ds_bug_motion_que`, 20
 * bytes each): the state of an alien flying along its program.
 */
#ifndef GALAGA_FLIGHT_SLOT_H
#define GALAGA_FLIGHT_SLOT_H

#include <stdint.h>

#include "cartridge/private.h"

/** A flight slot; the field names follow the meaning of the original bytes. */
typedef struct {
    uint8_t used;     /**< the slot is taken (the web version's `undefined` is a free slot) */
    uint16_t y;       /**< height (b00-b01), 9.7 fixed point, growing upwards */
    uint16_t x;       /**< horizontal position (b02-b03), 9.7 fixed point */
    uint16_t angle;   /**< direction (b04-b05): 10 bits, 0 right, $100 up, $200 left */
    uint8_t target_y; /**< target of "go home" or "dive until" (b06, b07), units of 2 pixels */
    uint8_t target_x;
    uint16_t pointer;  /**< address of the next byte of the flight program (b08-b09) */
    uint8_t speed_odd; /**< speed on odd and on even frames (b0A, b0B), pixels per frame */
    uint8_t speed_even;
    uint8_t turn;       /**< rotation per frame (b0C), a signed byte */
    uint8_t timer;      /**< frames left in the current instruction (b0D) */
    uint8_t bomb_timer; /**< frames to the next chance to drop a bomb (b0E) */
    uint8_t bomb_bits;  /**< the bits allowing it (b0F) */
    uint8_t object;     /**< the object flying (b10) */
    uint8_t offset_x;   /**< formation offset frozen when heading home (b11, b12) */
    uint8_t offset_y;
    uint8_t dive_check; /**< b13 bit 5: watching for the dive target */
    uint8_t homing;     /**< b13 bit 6: heading home */
    uint8_t mirrored;   /**< b13 bit 7: the program is mirrored */
} FlightSlot;

/** How the sprite of a flying alien looks for a given direction. */
typedef struct {
    int32_t tile; /**< which of the 8 drawings of the alien: 0-5 rotated, 6 upright */
    int flip_x;
    int flip_y;
} Pose;

/** A position in 9.7 fixed point. */
typedef struct {
    int32_t x;
    int32_t y;
} FlightPoint;

/** A sprite position. */
typedef struct {
    int32_t x;
    int32_t y;
} SpritePoint;

/** A free slot. */
GAME_PRIVATE FlightSlot free_slot(void);

/** A slot as the program prepares it before the first instruction is read: the rest is 0. */
GAME_PRIVATE FlightSlot new_slot(int32_t object);

/** The high byte of a 9.7 coordinate: the position in units of 2 pixels. */
GAME_PRIVATE int32_t coarse(int32_t value);

/**
 * `l_0C6D`: the drawing for `angle`. Each quarter turn has 6 drawings 15 degrees apart (42 angle
 * steps each), plus the upright one within 7.5 degrees of vertical; the other three quarters
 * mirror the first.
 */
GAME_PRIVATE Pose pose_for(int32_t angle);

/**
 * `l_0C46`-`l_0CFA`: one step of movement at `speed` pixels in direction `angle`. The axis closer
 * to the direction moves by the whole speed, the other by a fraction of it that grows with the
 * angle inside the octant: sine and cosine on an octagon, with one 8 x 8 bit multiplication.
 */
GAME_PRIVATE FlightPoint step_position(int32_t x, int32_t y, int32_t angle, int32_t speed);

/** The sprite position of a slot (`l_0D03`): flight y grows upwards, sprite y downwards. */
GAME_PRIVATE SpritePoint sprite_position(const FlightSlot *slot);

#endif
