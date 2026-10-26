/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test: the web version's tuning.config.ts and
 * colors.config.ts, and the FM-1's controls. The tables are in config.c.
 */
#ifndef BREAKOUT_CONFIG_H
#define BREAKOUT_CONFIG_H

#include <stdint.h>

#include "cartridge/private.h"
#include "input/fm1_panel.h"
#include "screen/framebuffer.h"

/** Balls per game: 3, or 5 with a switch inside the cabinet. */
#define BALLS_PER_GAME 3

/**
 * One row of the ball's speeds, in fifths per frame (tuning.config.ts ballSpeeds): `vertical`
 * in steps, sideways in scan lines after an outer or a middle segment of the paddle. The row
 * that applies is the last one whose `from_hits` the hit counter has reached.
 */
typedef struct {
    uint8_t from_hits;
    uint8_t vertical;
    uint8_t outer;
    uint8_t middle;
} SpeedRow;

/** How many rows BALL_SPEEDS has. */
#define SPEED_ROWS 4

/** 0.6/1.2/0.6, 0.8/1.2/0.6, 0.6/1.6/1.6, 0.8/1.6/1.6 at 0, 4, 8 and 12 hits. */
GAME_EXTERN const SpeedRow BALL_SPEEDS[SPEED_ROWS];

/** The fastest speed, after an orange or red brick: 1 down, 1.6 sideways. */
#define FAST_VERTICAL 5
#define FAST_SIDEWAYS 8

/**
 * The serve: the ball circles invisibly and shows up when it crosses the middle of the screen,
 * up to SERVE_CYCLE_FRAMES frames (about 4 seconds) after SERVE.
 */
#define SERVE_CYCLE_FRAMES 256u
#define SERVE_APPEAR_Y 120

/** How far the arrow keys of the web version move the paddle in one frame, in scan lines. */
#define PADDLE_KEY_SPEED 3

/** The film strip over the paddle area. */
#define PADDLE_STRIP_TOP 180
#define PADDLE_STRIP_HEIGHT 20

/** Dark gap between two rows of bricks, in steps. */
#define BRICK_ROW_GAP 1

/** The player's score blinks during play, 4 times a second at 60 frames a second. */
#define SCORE_BLINK_HZ 4u
#define FRAMES_PER_SECOND 60u

/** Frames between two brick ticks: a red brick sounds 7 ticks, one every 5 frames. */
#define TICK_FRAMES 5u

/** Whether the top wall sounds too, as the manual says (the netlist wires only the sides). */
#define TOP_WALL_SOUND 1

/** The beeps: frequency in hertz and length in milliseconds; the volume in thousandths. */
#define PADDLE_BEEP_HZ 2000u
#define PADDLE_BEEP_MS 9u
#define WALL_BEEP_HZ 1000u
#define WALL_BEEP_MS 21u
#define BRICK_BEEP_HZ 500u
#define BRICK_BEEP_MS 9u
#define BEEP_VOLUME 150u

/** The seven-segment digits of the score: size, strokes, pitch and places. */
#define DIGIT_WIDTH 10
#define DIGIT_HEIGHT 12
#define DIGIT_STROKE_X 2
#define DIGIT_STROKE_Y 2
#define DIGIT_PITCH 16
#define DIGITS_LEFT_X 36
#define DIGITS_RIGHT_X 144
#define DIGITS_UPPER_Y 10
#define DIGITS_LOWER_Y 26

/**
 * The FM-1's controls (Luca, 2026-10-25): any of the seven encoders moves the paddle, as the
 * cabinet's knob did; each click moves it KNOB_PIXELS scan lines. PLAY is SERVE (and starts a
 * game), FX switches between the coloured film and the black and white monitor.
 */
#define KNOB_PIXELS 4
#define SERVE_BUTTON FM1_BUTTON_PLAY
#define COLOR_BUTTON FM1_BUTTON_FX

/** The colour indexes of the screen: everything is drawn in ink, then the film tints it. */
enum {
    COLOR_BACKGROUND,
    COLOR_INK,
    COLOR_RED_STRIP,
    COLOR_ORANGE_STRIP,
    COLOR_GREEN_STRIP,
    COLOR_YELLOW_STRIP,
    COLOR_PADDLE_STRIP,
};

/** The colours of the film on the glass, from the MAME artwork (colors.config.ts). */
GAME_EXTERN const uint16_t COLORS[FB_COLORS];

/** The black and white monitor: no film. */
GAME_EXTERN const uint16_t MONO_COLORS[FB_COLORS];

/**
 * The line at the bottom of the attract screen, under the paddle (rows 200-207 are empty): the
 * cabinet needed none, but on the synth it says which controls play; below the paddle strip.
 */
#define ATTRACT_TEXT "PLAY SERVES  KNOBS MOVE"
#define ATTRACT_TEXT_Y 200

#endif
