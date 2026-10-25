/* SPDX-License-Identifier: GPL-3.0-only */
#include "hud.h"
#include "test/check.h"

static void the_score_shows_three_digits(void)
{
    char text[HUD_SCORE_CHARS];
    hud_format_score(7, text);
    CHECK_EQ(text[0], '0');
    CHECK_EQ(text[2], '7');
    hud_format_score(1234, text);
    CHECK_EQ(text[0], '2');
    CHECK_EQ(text[3], '\0');
}

static void the_score_blinks_four_times_a_second(void)
{
    CHECK(hud_blink_on(0));
    CHECK(hud_blink_on(7));
    CHECK(!hud_blink_on(8));
    CHECK(hud_blink_on(15));
}

int main(void)
{
    RUN(the_score_shows_three_digits);
    RUN(the_score_blinks_four_times_a_second);
    return check_report();
}
