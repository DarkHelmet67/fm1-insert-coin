/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_world.h"

void flight_emit(FlightWorld *world, FlightEventKind kind, int32_t slot, int32_t target_x)
{
    if (world->events->count >= MAX_EVENTS)
        return;
    FlightEvent *event = &world->events->list[world->events->count++];
    event->kind = (uint8_t)kind;
    event->slot = (uint8_t)slot;
    event->target_x = (uint8_t)target_x;
}
