/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `f_2000`, the rescue (rescue.ts). A Boss carrying a captured fighter was shot while diving:
 * the fighter, white again, spins in place, then glides to the middle of the screen and down
 * next to the player's fighter. The two join into the double fighter; if the player's fighter
 * was lost meanwhile, the rescued one simply takes its place.
 */
#ifndef GALAGA_RESCUE_H
#define GALAGA_RESCUE_H

#include "cartridge/private.h"
#include "playing.h"

/** `f_2000`: one frame of the rescue. */
GAME_PRIVATE void run_rescue(PlayingState *state);

#endif
