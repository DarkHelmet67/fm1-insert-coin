/* SPDX-License-Identifier: GPL-3.0-only */
#include <string.h>

#include "hud.h"
#include "palette.h"
#include "screen/draw.h"
#include "test/check.h"

static Framebuffer fb;

/** Pixels of character cell (column, row) that are not background. */
static int32_t lit_in_cell(int32_t column, int32_t row)
{
    int32_t n = 0;
    for (int32_t y = row * 8; y < row * 8 + 8; y++)
        for (int32_t x = column * 8; x < column * 8 + 8; x++)
            n += fb_get(&fb, x, y) != COLOR_BACKGROUND;
    return n;
}

static void a_score_is_right_aligned_with_at_least_two_digits(void)
{
    char text[7];
    CHECK(strcmp(score_text(0, text), "    00") == 0);
    CHECK(strcmp(score_text(7, text), "    07") == 0);
    CHECK(strcmp(score_text(1230, text), "  1230") == 0);
    CHECK(strcmp(score_text(999990, text), "999990") == 0);
}

static void one_up_blinks_off_and_the_scores_stay(void)
{
    fb_init(&fb, 224, 288);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_scores(&fb, 50, 20000, 1);
    CHECK_EQ(lit_in_cell(2, 0), 0);
    CHECK(lit_in_cell(9, 0) > 0); /* the H of HIGH SCORE */
    CHECK(lit_in_cell(5, 1) > 0); /* the last digit of the score */
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_scores(&fb, 50, 20000, 0);
    CHECK(lit_in_cell(2, 0) > 0);
}

static void at_most_five_reserve_fighters_show_at_the_bottom_left(void)
{
    fb_init(&fb, 224, 288);
    fb_clear(&fb, COLOR_BACKGROUND);
    draw_reserve(&fb, 7);
    CHECK(lit_in_cell(8, 34) + lit_in_cell(9, 34) + lit_in_cell(8, 35) + lit_in_cell(9, 35) > 0);
    CHECK_EQ(lit_in_cell(10, 34) + lit_in_cell(11, 35), 0);
}

int main(void)
{
    RUN(a_score_is_right_aligned_with_at_least_two_digits);
    RUN(one_up_blinks_off_and_the_scores_stay);
    RUN(at_most_five_reserve_fighters_show_at_the_bottom_left);
    return check_report();
}
