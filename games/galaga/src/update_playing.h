/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One frame of a game (update-playing.ts): the main program, then the tasks it has turned on, in
 * the order the two processors run them. Each task is a small function of the frame's working
 * copy of the state (see flight.h).
 */
#ifndef GALAGA_UPDATE_PLAYING_H
#define GALAGA_UPDATE_PLAYING_H

#include "cartridge/private.h"
#include "controls.h"
#include "playing.h"

/** One frame of play. */
GAME_PRIVATE void update_playing(PlayingState *state, const Controls *controls);

#endif
