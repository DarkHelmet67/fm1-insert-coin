/* SPDX-License-Identifier: GPL-3.0-only */
#include "bombs.h"

void set_up_bombs(Objects *objects)
{
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        GameObject *bomb = object_ref(objects, FIRST_BOMB + i * 2);
        bomb->code = BOMB_CODE;
        bomb->color = BOMB_COLOR;
        bomb->flip_x = 0;
        bomb->flip_y = 1;
        bomb->x = 0;
    }
}

void move_bombs(Objects *objects, BombDrift drift[BOMB_COUNT], int32_t frame)
{
    int32_t dy = (frame & 1) + 2;
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        GameObject *bomb = object_ref(objects, FIRST_BOMB + i * 2);
        if (bomb->code != BOMB_CODE || bomb->x == 0)
            continue;
        int32_t sum = (drift[i].rate & 0x7e) + drift[i].accumulator;
        int32_t pixels = (sum >> 5) & 7;
        int32_t dx = (drift[i].rate & 0x80) != 0 ? -pixels : pixels;
        bomb->x = (uint8_t)(bomb->x + dx);
        bomb->y = (uint16_t)((bomb->y + dy) & 0x1ff);
        drift[i].accumulator = (uint8_t)(sum & 0x1f);
    }
}
