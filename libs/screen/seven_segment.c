/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/seven_segment.h"

/** The lit segments of 0-9, bit n for segment n (a = bit 0 ... g = bit 6). */
static const uint8_t DIGIT_BITS[10] = {
    0x3F, /* 0: a b c d e f */
    0x06, /* 1: b c */
    0x5B, /* 2: a b d e g */
    0x4F, /* 3: a b c d g */
    0x66, /* 4: b c f g */
    0x6D, /* 5: a c d f g */
    0x7D, /* 6: a c d e f g */
    0x07, /* 7: a b c */
    0x7F, /* 8: all */
    0x6F, /* 9: a b c d f g */
};

uint8_t seven_segment_bits(char digit)
{
    return digit >= '0' && digit <= '9' ? DIGIT_BITS[digit - '0'] : 0u;
}

/** A box from its four numbers. */
static SegmentBox box(int32_t x, int32_t y, int32_t width, int32_t height)
{
    SegmentBox b = {x, y, width, height};
    return b;
}

SegmentBox seven_segment_box(Segment segment, const SegmentStyle *style)
{
    const int32_t w = style->width;
    const int32_t h = style->height;
    const int32_t sx = style->stroke_x;
    const int32_t sy = style->stroke_y;
    /* the middle bar's top row; the upper verticals reach down to its bottom, the lower ones
     * start at its top, so each half of the digit is closed */
    const int32_t middle = (h - sy) / 2;
    const int32_t upper = middle + sy;
    const int32_t lower = h - middle;
    switch (segment) {
    case SEGMENT_A:
        return box(0, 0, w, sy);
    case SEGMENT_B:
        return box(w - sx, 0, sx, upper);
    case SEGMENT_C:
        return box(w - sx, middle, sx, lower);
    case SEGMENT_D:
        return box(0, h - sy, w, sy);
    case SEGMENT_E:
        return box(0, middle, sx, lower);
    case SEGMENT_F:
        return box(0, 0, sx, upper);
    default:
        return box(0, middle, w, sy);
    }
}

void fb_draw_segment_digit(Framebuffer *fb, char digit, int32_t x, int32_t y,
                           const SegmentStyle *style, uint8_t color)
{
    const uint8_t bits = seven_segment_bits(digit);
    for (int segment = SEGMENT_A; segment <= SEGMENT_G; segment++) {
        if (!(bits >> segment & 1u))
            continue;
        const SegmentBox b = seven_segment_box((Segment)segment, style);
        fb_fill_rect(fb, x + b.x, y + b.y, b.width, b.height, color);
    }
}

void fb_draw_segment_number(Framebuffer *fb, const char *digits, int32_t x, int32_t y,
                            int32_t pitch, const SegmentStyle *style, uint8_t color)
{
    for (int32_t i = 0; digits[i] != '\0'; i++)
        fb_draw_segment_digit(fb, digits[i], x + i * pitch, y, style, color);
}
