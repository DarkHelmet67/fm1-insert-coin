/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_23DD`, the object manager (object-states.ts). Every 4 frames each object takes one step of
 * what its disposition says: resting aliens follow the grid and flap their wings, aliens back
 * home turn upright, explosions advance, things that left the screen are removed. Half of the
 * objects are handled on frame 1, the other half on frame 3; on frame 2 the count of aliens in
 * play is published, the number the rest of the game uses to know when a stage is cleared.
 */
#ifndef GALAGA_OBJECT_STATES_H
#define GALAGA_OBJECT_STATES_H

#include <stdint.h>

#include "cartridge/private.h"
#include "flight_world.h"
#include "formation.h"

/** What the object manager changes: parts of the game's working copy (see flight.h). */
typedef struct {
    Objects *objects;
    Slots *slots;
    int32_t *counting; /**< aliens counted so far in this round of 4 frames (`b_bugs_actv_cnt`) */
    int32_t *alive;    /**< aliens in play at the last count (`b_bugs_actv_nbr`) */
} ObjectWorld;

/** What the object manager reads. */
typedef struct {
    int32_t frame;
    const Formation *formation;
    int enemies_enabled; /**< resting aliens follow the grid (`ds_9200_glbls[$0B]`) */
} ObjectInputs;

/** The score sprites of 1000 points and more use other palettes (`l_24FD`). */
GAME_PRIVATE int32_t score_palette(int32_t code, int32_t color);

/** One frame of the object manager. */
GAME_PRIVATE void update_object_states(const ObjectWorld *world, const ObjectInputs *inputs);

#endif
