/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test (as tuning.config.ts in the web version).
 */
#ifndef SPACE_INVADERS_CONFIG_H
#define SPACE_INVADERS_CONFIG_H

#include "input/fm1_keys.h"
#include "input/keymap.h"

/**
 * The controls (Luca, 2026-10-24): the first three labelled black keys move left, the next two
 * fire, the next three move right, so the left hand steers left, the right hand steers right and
 * either thumb can fire.
 */
static const KeyMap CONTROLS = {
    .left = FM1_KEY_BIT(FM1_KEY_OP1) | FM1_KEY_BIT(FM1_KEY_OP2) | FM1_KEY_BIT(FM1_KEY_OP3),
    .fire = FM1_KEY_BIT(FM1_KEY_OP4) | FM1_KEY_BIT(FM1_KEY_OP5),
    .right = FM1_KEY_BIT(FM1_KEY_OP6) | FM1_KEY_BIT(FM1_KEY_PIT) | FM1_KEY_BIT(FM1_KEY_GLO),
};

#endif
