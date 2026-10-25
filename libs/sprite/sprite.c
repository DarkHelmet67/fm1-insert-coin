/* SPDX-License-Identifier: GPL-3.0-only */
#include "sprite.h"

int sprite_lit(const Sprite *sprite, int32_t x, int32_t y)
{
    if (x < 0 || y < 0 || x >= sprite->width || y >= sprite->height)
        return 0;
    return sprite->rows[y][x] == 'X';
}

int sprite_is_well_formed(const Sprite *sprite)
{
    for (uint32_t y = 0; y < sprite->height; y++) {
        uint32_t length = 0;
        while (sprite->rows[y][length])
            length++;
        if (length != sprite->width)
            return 0;
    }
    return 1;
}
