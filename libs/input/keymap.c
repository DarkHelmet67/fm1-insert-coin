/* SPDX-License-Identifier: GPL-3.0-only */
#include "keymap.h"

uint32_t keymap_actions(const KeyMap *map, uint32_t notes)
{
    const uint32_t left = (notes & map->left) ? ACTION_LEFT : 0u;
    const uint32_t right = (notes & map->right) ? ACTION_RIGHT : 0u;
    const uint32_t fire = (notes & map->fire) ? ACTION_FIRE : 0u;
    const uint32_t steer = (left && right) ? 0u : (left | right);
    return steer | fire;
}

uint32_t keymap_keys(const KeyMap *map)
{
    return map->left | map->right | map->fire;
}
