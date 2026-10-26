/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_bombs.h"

#include "z80_math.h"

/** Bombs fall only from aliens above this height (coarse flight units). */
#define BOMB_LOWEST 0x4c

int32_t bomb_drift_rate(int32_t bomb, const GameObject *bomber, int32_t fighter_x)
{
    int32_t dx = (fighter_x - bomber->x) & 0xff;
    int left = fighter_x < bomber->x;
    int32_t distance_x = left ? -dx & 0xff : dx;
    int32_t height = (0x95 - (bomber->y >> 1)) & 0xff;
    int32_t distance_y = bomber->y >> 1 > 0x95 ? -height & 0xff : height;
    /* The low byte of the dividend is whatever was in L: the bomb's own register address. */
    int32_t ratio = z80_divide((distance_x << 8) | ((bomb + 1) & 0xff), distance_y);
    int32_t rate = (ratio + (ratio >> 2)) >> 2;
    if (rate > 0x60)
        rate = 0x60;
    return (left << 7) | (rate >> 1);
}

/** The first inactive bomb, or -1. */
static int32_t free_bomb(const Objects *objects)
{
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        int32_t id = FIRST_BOMB + i * 2;
        if (object_at(objects, id)->disposition == DISPOSITION_INACTIVE)
            return id;
    }
    return -1;
}

void try_bombing(FlightWorld *world, FlightSlot *slot, const FlightInputs *inputs)
{
    slot->bomb_timer = (uint8_t)(slot->bomb_timer - 1);
    if (slot->bomb_timer != 0)
        return;
    int allowed = (slot->bomb_bits & 1) == 1;
    slot->bomb_bits = (uint8_t)(slot->bomb_bits >> 1);
    slot->bomb_timer = (uint8_t)inputs->bomb_interval;
    int32_t bomb = free_bomb(world->objects);
    if (!allowed || coarse(slot->y) < BOMB_LOWEST || !inputs->bombs_allowed || bomb < 0)
        return;
    const GameObject *bomber = object_at(world->objects, slot->object);
    GameObject *dropped = object_ref(world->objects, bomb);
    BombDrift *drift = &world->bomb_drift[(bomb - FIRST_BOMB) >> 1];
    dropped->disposition = DISPOSITION_SHOT;
    dropped->x = bomber->x;
    dropped->y = bomber->y;
    drift->rate = (uint8_t)bomb_drift_rate(bomb, bomber, inputs->fighter_x);
    drift->accumulator = 0;
}
