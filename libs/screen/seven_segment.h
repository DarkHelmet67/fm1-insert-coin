/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Numbers in seven-segment digits, as the score displays of the 1970s drew them (insert-coin,
 * @arcade/render seven-segment.ts): each digit is a few filled rectangles.
 */
#ifndef SEVEN_SEGMENT_H
#define SEVEN_SEGMENT_H

#include <stdint.h>

#include "screen/framebuffer.h"

/**
 * The seven segments, named as in every datasheet: a top, b top right, c bottom right,
 * d bottom, e bottom left, f top left, g middle.
 */
typedef enum {
    SEGMENT_A,
    SEGMENT_B,
    SEGMENT_C,
    SEGMENT_D,
    SEGMENT_E,
    SEGMENT_F,
    SEGMENT_G
} Segment;

/** The size of a digit: outer width and height, and the thickness of its segments. */
typedef struct {
    int32_t width;
    int32_t height;
    int32_t stroke_x; /**< thickness of the vertical segments (b, c, e, f) */
    int32_t stroke_y; /**< thickness of the horizontal segments (a, d, g) */
} SegmentStyle;

/** A segment's rectangle, from the digit's top left corner. */
typedef struct {
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
} SegmentBox;

/** The segments a digit lights, as bits (1 << SEGMENT_A ...), as a 7448 decoder does; 0 for
 * anything but '0'-'9'. */
uint8_t seven_segment_bits(char digit);

/**
 * Where a segment sits in the digit. The vertical segments overlap the horizontal ones at the
 * corners, so the outline is closed as on a real display.
 */
SegmentBox seven_segment_box(Segment segment, const SegmentStyle *style);

/** Draws one digit with its top left corner at (x, y); other characters draw nothing. */
void fb_draw_segment_digit(Framebuffer *fb, char digit, int32_t x, int32_t y,
                           const SegmentStyle *style, uint8_t color);

/** Draws a string of digits, one every `pitch` pixels from `x`. */
void fb_draw_segment_number(Framebuffer *fb, const char *digits, int32_t x, int32_t y,
                            int32_t pitch, const SegmentStyle *style, uint8_t color);

#endif
