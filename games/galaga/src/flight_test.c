/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdlib.h>

#include "flight.h"
#include "formation.h"
#include "objects.h"
#include "test/check.h"

static Slots slots;
static Objects objects;
static BombDrift drift[BOMB_COUNT];
static FlightEvents events;
static Formation formation;

/** Flight inputs of a quiet first stage. */
static FlightInputs inputs(int32_t frame)
{
    const FlightInputs in = {frame, 0x7a, 0, 0, 0, 0, 0, 0x14, 0, &formation, 0x1b};
    return in;
}

/** A bee entering from the top left on the first stage's first program. */
static FlightWorld entering(void)
{
    const FlightWorld world = {&slots, &objects, drift, &events};
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        slots.list[i] = free_slot();
    slots.list[0] = new_slot(0x08);
    slots.list[0].y = 0x9b << 8;
    slots.list[0].x = 0x34 << 8;
    slots.list[0].angle = 0x300;
    slots.list[0].pointer = 0x001d;
    objects = empty_objects();
    GameObject *bee = object_ref(&objects, 0x08);
    bee->disposition = DISPOSITION_ENTERING;
    bee->code = 0x18;
    bee->color = 3;
    for (int32_t i = 0; i < BOMB_COUNT; i++)
        drift[i].rate = drift[i].accumulator = 0;
    events.count = 0;
    formation = initial_formation();
    return world;
}

static void an_entering_bee_flies_into_its_place_in_the_formation(void)
{
    FlightWorld world = entering();
    int32_t frame = 0;
    while (slots.list[0].used && frame < 1000) {
        const FlightInputs in = inputs(frame);
        run_flights(&world, &in);
        frame++;
    }
    const GameObject *bee = object_at(&objects, 0x08);
    CHECK(frame < 1000);
    CHECK_EQ(bee->disposition, DISPOSITION_ROTATING);
    /* column 0 of the grid is at x = $31, row 4 (the upper bee row) at sprite y 116 */
    CHECK(abs(bee->x - 0x31) <= 1);
    CHECK(abs(bee->y - 116) <= 2);
}

static void a_flying_aliens_sprite_moves_on_the_screen(void)
{
    FlightWorld world = entering();
    for (int32_t frame = 0; frame < 2; frame++) {
        const FlightInputs in = inputs(frame);
        run_flights(&world, &in);
    }
    CHECK(object_at(&objects, 0x08)->x > 0);
    CHECK(object_at(&objects, 0x08)->y > 0);
}

int main(void)
{
    RUN(an_entering_bee_flies_into_its_place_in_the_formation);
    RUN(a_flying_aliens_sprite_moves_on_the_screen);
    return check_report();
}
