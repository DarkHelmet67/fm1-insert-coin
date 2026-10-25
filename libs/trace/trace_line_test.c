/* SPDX-License-Identifier: GPL-3.0-only */
#include "test/check.h"
#include "trace/trace_line.h"

static void a_frame_line_is_read(void)
{
    TraceLine line;
    CHECK(trace_parse_line("1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18\n", &line));
    CHECK_EQ(line.frame, 1234u);
    CHECK_EQ(line.direction, 1);
    CHECK(line.fire);
    CHECK(!line.coin);
    CHECK_EQ(line.screen, 'P');
    CHECK_EQ(line.hash, 0x0B4BF1D3u);
}

static void the_attract_screen_has_no_numbers(void)
{
    TraceLine line;
    CHECK(trace_parse_line("60 --C A e8c203ae", &line));
    CHECK_EQ(line.direction, 0);
    CHECK(line.coin);
    CHECK_EQ(line.screen, 'A');
}

static void moving_left_on_the_game_over_screen(void)
{
    TraceLine line;
    CHECK(trace_parse_line("9 L-- G 00000001", &line));
    CHECK_EQ(line.direction, -1);
    CHECK_EQ(line.screen, 'G');
    CHECK_EQ(line.hash, 1u);
}

static void comments_and_bad_lines_are_skipped(void)
{
    TraceLine line;
    CHECK(!trace_parse_line("# initial e8c203ae", &line));
    CHECK(!trace_parse_line("", &line));
    CHECK(!trace_parse_line("12 X-- P 0b4bf1d3", &line));
    CHECK(!trace_parse_line("12 --- P 0b4bf1", &line));
    CHECK(!trace_parse_line("12 --- Q 0b4bf1d3", &line));
}

int main(void)
{
    RUN(a_frame_line_is_read);
    RUN(the_attract_screen_has_no_numbers);
    RUN(moving_left_on_the_game_over_screen);
    RUN(comments_and_bad_lines_are_skipped);
    return check_report();
}
