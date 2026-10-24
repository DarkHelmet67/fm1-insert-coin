/* SPDX-License-Identifier: GPL-3.0-only */
#include "shields.h"

#include "sprites.h"

Shields shields_create(void)
{
    Shields shields;
    const Bitmap intact = bitmap_from_sprite(&SHIELD_SPRITE);
    for (int32_t i = 0; i < SHIELD_COUNT; i++) {
        shields.list[i].x = (int16_t)(FIRST_SHIELD_X + i * SHIELD_SPACING);
        shields.list[i].y = SHIELD_Y;
        shields.list[i].bitmap = intact;
    }
    return shields;
}

/** A shield as a placed bitmap. */
static Placed placed_shield(const Shield *shield)
{
    Placed placed = {&shield->bitmap, shield->x, shield->y};
    return placed;
}

int shields_hit(const Shields *shields, const Placed *object)
{
    for (uint32_t i = 0; i < SHIELD_COUNT; i++) {
        const Placed shield = placed_shield(&shields->list[i]);
        if (placed_overlap(&shield, object))
            return 1;
    }
    return 0;
}

Shields shields_damage(const Shields *shields, const Placed *brush)
{
    Shields next = *shields;
    for (uint32_t i = 0; i < SHIELD_COUNT; i++) {
        const Placed shield = placed_shield(&shields->list[i]);
        if (placed_overlap(&shield, brush))
            next.list[i].bitmap = bitmap_erase(&shield, brush);
    }
    return next;
}
