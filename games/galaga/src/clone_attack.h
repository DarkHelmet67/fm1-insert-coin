/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_1A80`, the "clone attack" (clone-attack.ts), from stage 3 on. When fewer aliens than the
 * stage parameter are left, a resting bee (or butterfly) is chosen as "bonus bee": it flashes in
 * a new color for 64 frames, then dives, and on the way splits off two special enemies (`$F2` in
 * its program). Shooting all three is worth 1000, 2000 or 3000 points, shown where the last one
 * falls.
 */
#ifndef GALAGA_CLONE_ATTACK_H
#define GALAGA_CLONE_ATTACK_H

#include "cartridge/private.h"
#include "playing.h"

/** `f_1A80`: one frame of the clone attack, on once the waves are home. */
GAME_PRIVATE void clone_attack(PlayingState *state);

#endif
