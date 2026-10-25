/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The tractor beam is made of characters (draw-beam.ts, `f_2222`, `d_23A1`): 10 rows of 6 cells
 * under the Boss, centered on the column the Boss aimed at. Its palette cycles every 4 frames.
 */
#ifndef GALAGA_BEAM_H
#define GALAGA_BEAM_H

#include <stdint.h>

#include "capture_state.h"
#include "cartridge/private.h"
#include "screen/framebuffer.h"

/**
 * `c_238A`: the leftmost column of the beam. The program computes a character address from the
 * sprite x of the beam's center; the screen is rotated, so addresses run along the columns.
 */
GAME_PRIVATE int32_t beam_column(int32_t beam_x);

/** The palette of the beam at `frame`: $18, $19, $1A in turn (`l_2246`). */
GAME_PRIVATE int32_t beam_palette(int32_t frame);

/** Draws the rows of the beam drawn so far. */
GAME_PRIVATE void draw_beam(Framebuffer *fb, const Capture *capture, int32_t frame);

#endif
