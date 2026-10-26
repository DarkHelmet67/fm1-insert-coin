/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The capturing Boss and its tractor beam (tractor-beam.ts, `f_21CB`, `f_2222`). The Boss dives
 * to the middle of the screen above the fighter, turns upright and opens the beam one row of
 * characters at a time. The beam stays open for 64 frames: a fighter within 27 pixels of its
 * center is caught. Then the beam closes from the bottom; a Boss shot while beaming closes it
 * from the top.
 */
#ifndef GALAGA_TRACTOR_BEAM_H
#define GALAGA_TRACTOR_BEAM_H

#include "cartridge/private.h"
#include "playing.h"

/**
 * `f_21CB`: the Boss dives until its program stops it (speed 0), then turns 12 steps a frame
 * until it points up: the beam starts.
 */
GAME_PRIVATE void approach(PlayingState *state);

/**
 * `l_233D`: while the beam is fully open, a fighter within 27 pixels of its center is caught.
 * The joystick and the collisions stop, the stars run backwards.
 */
GAME_PRIVATE void try_to_catch(PlayingState *state);

/** `f_2222`: one frame of the beam. */
GAME_PRIVATE void run_beam(PlayingState *state);

#endif
