/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/seven_segment.h"
#include "test/check.h"

static Framebuffer fb;

static const SegmentStyle STYLE = {10, 12, 2, 2};

static void the_digits_light_the_decoder_segments(void)
{
    CHECK_EQ(seven_segment_bits('8'), 0x7F);
    CHECK_EQ(seven_segment_bits('1'), (1 << SEGMENT_B) | (1 << SEGMENT_C));
    CHECK_EQ(seven_segment_bits('0') >> SEGMENT_G & 1, 0);
    CHECK_EQ(seven_segment_bits('x'), 0);
}

static void the_verticals_close_both_halves(void)
{
    /* middle bar at rows 5-6: the upper verticals end on row 6, the lower ones start on row 5 */
    const SegmentBox g = seven_segment_box(SEGMENT_G, &STYLE);
    const SegmentBox b = seven_segment_box(SEGMENT_B, &STYLE);
    const SegmentBox c = seven_segment_box(SEGMENT_C, &STYLE);
    CHECK_EQ(g.y, 5);
    CHECK_EQ(b.x, 8);
    CHECK_EQ(b.y + b.height, 7);
    CHECK_EQ(c.y, 5);
    CHECK_EQ(c.y + c.height, 12);
}

static void a_number_is_drawn_digit_by_digit(void)
{
    fb_init(&fb, 64, 16);
    fb_draw_segment_number(&fb, "10", 2, 1, 16, &STYLE, 3);
    CHECK_EQ(fb_get(&fb, 2 + 9, 1), 3);  /* the 1's right stroke */
    CHECK_EQ(fb_get(&fb, 2, 1), 0);      /* the 1 has no top bar */
    CHECK_EQ(fb_get(&fb, 18, 1), 3);     /* the 0's top bar */
    CHECK_EQ(fb_get(&fb, 18 + 4, 7), 0); /* the 0 is open in the middle */
}

int main(void)
{
    RUN(the_digits_light_the_decoder_segments);
    RUN(the_verticals_close_both_halves);
    RUN(a_number_is_drawn_digit_by_digit);
    return check_report();
}
