/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * From the keys held to what the player asks for: each game says which keys mean left, right and
 * fire, and reads actions instead of keys.
 */
#ifndef KEYMAP_H
#define KEYMAP_H

#include <stdint.h>

/** What a player can ask for, one bit each. */
enum {
    ACTION_LEFT = 1u << 0,
    ACTION_RIGHT = 1u << 1,
    ACTION_FIRE = 1u << 2,
};

/** For each action, the keys that ask for it (bits of `fm1_in.notes`, see fm1_keys.h). */
typedef struct {
    uint32_t left;
    uint32_t right;
    uint32_t fire;
} KeyMap;

/**
 * The actions asked for by the keys held now.
 * @param map   which keys mean what
 * @param notes the keys held, bits of `fm1_in.notes`
 * @returns ACTION_* bits; left and right together cancel out, as two hands pushing a stick both
 * ways
 */
uint32_t keymap_actions(const KeyMap *map, uint32_t notes);

/**
 * Every key the map uses, to light their LEDs.
 * @param map which keys mean what
 * @returns bits of `fm1_in.notes`
 */
uint32_t keymap_keys(const KeyMap *map);

#endif
