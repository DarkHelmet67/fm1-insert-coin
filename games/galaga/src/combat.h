/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The fight, as tasks of a frame (combat.ts): aliens leave the formation (`f_1B65`), bombs fall
 * (`f_1EA4`), rockets and bombs hit (`hitd_*`), hit aliens explode (`f_1DB3`) and the hit
 * counters become points. Each task changes the frame's working copy of the game, like the
 * others in update_playing.h.
 */
#ifndef GALAGA_COMBAT_H
#define GALAGA_COMBAT_H

#include "bomber_timers.h"
#include "cartridge/private.h"
#include "playing.h"

/** Whether the aliens attack nonstop: few are left and the fighter can fire. */
GAME_PRIVATE int is_continuous(const PlayingState *state);

/** `f_0857`: the bomber settings of this moment. */
GAME_PRIVATE BomberSettings current_settings(const PlayingState *state);

/** `f_1B65`: when its task is on, an alien may leave the formation. */
GAME_PRIVATE void attack(PlayingState *state);

/** `f_1DB3`: hit aliens start exploding. */
GAME_PRIVATE void explode(PlayingState *state);

/** `f_1EA4`: one frame of the bombs. */
GAME_PRIVATE void fall_bombs(PlayingState *state);

/** One frame of collisions; the hits are turned into points at once. */
GAME_PRIVATE void collide(PlayingState *state);

#endif
