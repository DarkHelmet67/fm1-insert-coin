/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The playfield of the 1976 cabinet, in the units of the original circuit (playfield.ts): x
 * counts scan lines across the rotated monitor, y counts steps of the horizontal counter along
 * it, from the top of the visible picture.
 */
#ifndef BREAKOUT_PLAYFIELD_H
#define BREAKOUT_PLAYFIELD_H

/** Screen size: 228 scan lines across, 208 counter steps top to bottom. */
#define SCREEN_WIDTH 228
#define SCREEN_HEIGHT 208

/** Width of the two side walls, in scan lines. */
#define SIDE_WALL_WIDTH 4

/** Thickness of the top ("back") wall. */
#define TOP_WALL_HEIGHT 8

/** Left edge of the right wall: the ball moves between the two walls. */
#define RIGHT_WALL_X (SCREEN_WIDTH - SIDE_WALL_WIDTH)

/** Bricks: 8 rows of 14, each 14 lines wide with a 2-line gap, 4 steps tall. */
#define BRICK_ROWS 8
#define BRICK_COLUMNS 14
#define BRICK_WIDTH 14
#define BRICK_PITCH 16
#define BRICK_HEIGHT 4

/** Top of the first brick row: the gap above it holds the scores. */
#define BRICKS_TOP 40

/** The ball: 4 scan lines wide, 2 steps tall. */
#define BALL_WIDTH 4
#define BALL_HEIGHT 2

/** The paddle row: 4 steps thick, near the bottom of the picture. */
#define PADDLE_Y 188
#define PADDLE_HEIGHT 4

/**
 * The ball moves in fifths of a scan line or step: every speed of the web version is a multiple
 * of 0.2, so counting fifths keeps its positions exact, with integers only.
 */
#define FIFTHS 5

#endif
