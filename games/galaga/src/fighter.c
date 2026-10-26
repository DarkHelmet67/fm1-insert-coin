/* SPDX-License-Identifier: GPL-3.0-only */
#include "fighter.h"

/** Leftmost position, and rightmost for a single and a double fighter (`c_1F92`). */
#define LEFT_LIMIT 0x12
#define RIGHT_LIMIT 0xe1
#define RIGHT_LIMIT_DUAL 0xd1

void place_fighter(Objects *objects)
{
    GameObject *fighter = object_ref(objects, FIGHTER);
    fighter->code = FIGHTER_UPRIGHT;
    fighter->color = FIGHTER_COLOR;
    fighter->x = FIGHTER_START_X;
    fighter->y = FIGHTER_START_Y;
    fighter->flip_x = 0;
    fighter->flip_y = 0;
    fighter->big = 0;
    fighter->wide = 0;
}

void move_fighter(Objects *objects, uint8_t *step_toggle, int32_t direction, int dual)
{
    if (direction == 0) {
        *step_toggle = 0;
        return;
    }
    *step_toggle = !*step_toggle;
    int32_t step = *step_toggle ? 1 : 2;
    GameObject *fighter = object_ref(objects, FIGHTER);
    int32_t x = fighter->x;
    if (x == 0)
        return;
    int blocked =
        direction > 0 ? (dual && x >= RIGHT_LIMIT_DUAL) || x >= RIGHT_LIMIT : x < LEFT_LIMIT;
    if (blocked)
        return;
    fighter->x = (uint8_t)(x + direction * step);
    if (dual)
        object_ref(objects, FIGHTER_2)->x = (uint8_t)(fighter->x + DUAL_SPACING);
}
