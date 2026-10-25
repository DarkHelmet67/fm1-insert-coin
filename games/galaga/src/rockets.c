/* SPDX-License-Identifier: GPL-3.0-only */
#include "rockets.h"

/** Pixels a rocket travels per frame along its main axis. */
#define ROCKET_SPEED 6

/** The object numbers of the two rockets. */
static const int32_t ROCKETS[ROCKET_COUNT] = {FIRST_ROCKET, FIRST_ROCKET + 2};

Rocket rocket_for(const GameObject *fighter)
{
    int32_t tile = fighter->code & 7;
    int vertical = tile >= 4;
    int32_t side = 2 * (vertical ? 6 - tile : tile);
    /* An unmirrored drawing points left (and up): mirroring sends the rocket right (and down). */
    int32_t sx = fighter->flip_x ? 1 : -1;
    int32_t sy = fighter->flip_y ? 1 : -1;
    Rocket rocket;
    rocket.code = tile >= 5 ? 0x30 : tile >= 2 ? 0x31 : 0x33;
    rocket.path.dx = (int8_t)(vertical ? sx * side : sx * ROCKET_SPEED);
    rocket.path.dy = (int8_t)(vertical ? sy * ROCKET_SPEED : sy * side);
    return rocket;
}

int fire_rocket(Objects *objects, RocketPaths *paths, int dual)
{
    const GameObject *fighter = object_at(objects, FIGHTER);
    int free = object_at(objects, ROCKETS[0])->x == 0   ? 0
               : object_at(objects, ROCKETS[1])->x == 0 ? 1
                                                        : -1;
    if (free < 0 || fighter->x == 0)
        return 0;
    Rocket rocket = rocket_for(fighter);
    GameObject *object = object_ref(objects, ROCKETS[free]);
    object->disposition = DISPOSITION_SHOT;
    object->code = (uint8_t)rocket.code;
    object->color = ROCKET_COLOR;
    object->x = fighter->x;
    object->y = fighter->y;
    object->flip_x = fighter->flip_x;
    object->flip_y = fighter->flip_y;
    object->wide = (uint8_t)dual;
    paths->path[free] = rocket.path;
    return 1;
}

/** A rocket leaves the play area above y = 40 or below 312, or past x = 240 (`rckt_man`). */
static int out_of_play(int32_t x, int32_t y)
{
    return x >= 0xf0 || y >> 1 < 0x28 >> 1 || y >> 1 >= 0x138 >> 1;
}

void move_rockets(Objects *objects, const RocketPaths *paths)
{
    for (int i = 0; i < ROCKET_COUNT; i++) {
        GameObject *rocket = object_ref(objects, ROCKETS[i]);
        if (rocket->x == 0)
            continue;
        int32_t x = (rocket->x + paths->path[i].dx) & 0xff;
        int32_t y = (rocket->y + paths->path[i].dy) & 0x1ff;
        if (out_of_play(x, y)) {
            /* A free rocket: off screen, ready to be fired again. */
            rocket->disposition = DISPOSITION_INACTIVE;
            rocket->x = 0;
            rocket->wide = 0;
        } else {
            rocket->x = (uint8_t)x;
            rocket->y = (uint16_t)y;
        }
    }
}

RocketPaths no_paths(void)
{
    RocketPaths paths = {{{0, -ROCKET_SPEED}, {0, -ROCKET_SPEED}}};
    return paths;
}
