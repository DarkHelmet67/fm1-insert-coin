/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The object table of the original program (objects.ts). Galaga keeps every moving thing in one
 * table of 64 objects, numbered by their offset in memory ($00, $02 ... $7E): the program's own
 * names and comments refer to them by that number, and so does the port.
 *
 *   $00-$06  fighters captured by the four Bosses     $38-$3E  "transients" (special enemies)
 *   $08-$2E  20 bees (Zako)                           $40-$5E  16 butterflies (Goei)
 *   $30-$36  4 Boss Galaga                            $60-$62  the player's fighters
 *   $64-$66  the 2 rockets                            $68-$76  the 8 bombs
 *
 * Each object has a "disposition" (what it is doing, table b_8800) and the registers of its
 * sprite: code, palette, position, mirroring and size.
 */
#ifndef GALAGA_OBJECTS_H
#define GALAGA_OBJECTS_H

#include <stdint.h>

#include "cartridge/private.h"

/** What an object is doing: the values the original program keeps in table `b_8800`. */
enum {
    DISPOSITION_HOME = 0x01,              /**< resting in the formation */
    DISPOSITION_ROTATING = 0x02,          /**< just arrived home: turning upright before resting */
    DISPOSITION_LAUNCHED = 0x03,          /**< flying a program that does not return home yet */
    DISPOSITION_EXPLODING = 0x04,         /**< hit: playing its explosion */
    DISPOSITION_SHOWING_SCORE = 0x05,     /**< showing the score of a hit */
    DISPOSITION_SHOT = 0x06,              /**< a rocket or a bomb in flight */
    DISPOSITION_ENTERING = 0x07,          /**< entering the screen in an attack wave */
    DISPOSITION_FIGHTER_EXPLODING = 0x08, /**< the player's fighter, exploding */
    DISPOSITION_FLYING = 0x09,            /**< diving, or heading back to the formation */
    DISPOSITION_INACTIVE = 0x80,          /**< not in play */
};

/** The registers of one object. */
typedef struct {
    uint8_t disposition;
    uint8_t counter; /**< the flight slot of a flying alien, or the step of an explosion */
    uint8_t code;    /**< sprite code, 0-127 */
    uint8_t color;   /**< sprite palette, 0-15 */
    uint8_t x;       /**< sprite position: x is 8 bits, y 9 bits; x = 0 hides the sprite */
    uint16_t y;
    uint8_t flip_x;
    uint8_t flip_y;
    uint8_t big;  /**< double size: four tiles in a 32 x 32 square (the big explosions) */
    uint8_t wide; /**< double width: two tiles side by side (the double fighter's rocket) */
    /**
     * Hit notification (`b_9200_obj_collsn_notif`): 0 when nothing happened, $81 when hit and
     * waiting to explode, or the code of the score sprite to show after the explosion.
     */
    uint8_t hit;
} GameObject;

/** Number of objects in the table. */
#define OBJECT_COUNT 64

/** The whole table, indexed by object number / 2. */
typedef struct {
    GameObject list[OBJECT_COUNT];
} Objects;

/** Object numbers used by name. */
#define FIGHTER 0x62
#define FIGHTER_2 0x60
#define FIRST_ROCKET 0x64
#define FIRST_BOMB 0x68
#define BOMB_COUNT 8
/** The aliens and captured fighters: objects $00-$5E, the ones rockets can hit (in halves). */
#define ENEMY_OBJECTS 0x30

/** An unused object, off screen. */
GAME_PRIVATE GameObject no_object(void);

/** A table of unused objects. */
GAME_PRIVATE Objects empty_objects(void);

/** Whether `id` is a number of the table ($00-$7F). */
GAME_PRIVATE int object_exists(int32_t id);

/** Object number `id`; outside the table, an unused object. */
GAME_PRIVATE const GameObject *object_at(const Objects *objects, int32_t id);

/**
 * Object number `id`, to change; NULL outside the table, where the web version's changes
 * find nothing to change.
 */
GAME_PRIVATE GameObject *object_ref(Objects *objects, int32_t id);

/** Whether the object is in play (its disposition is not "inactive"). */
GAME_PRIVATE int is_active(const GameObject *object);

#endif
