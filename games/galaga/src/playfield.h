/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The screen of the cabinet (playfield.ts): the monitor is turned upright, 224 x 288 pixels.
 */
#ifndef GALAGA_PLAYFIELD_H
#define GALAGA_PLAYFIELD_H

/** Screen size in pixels. */
#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 288

/** Screen size in 8 x 8 characters. */
#define SCREEN_COLUMNS (SCREEN_WIDTH / 8)
#define SCREEN_ROWS (SCREEN_HEIGHT / 8)

/**
 * The sprite hardware places a sprite at register position (x, y) with its top-left corner 16
 * pixels to the left and 40 pixels above: the positions of the program are converted only when
 * drawing.
 */
#define SPRITE_OFFSET_X 16
#define SPRITE_OFFSET_Y 40

#endif
