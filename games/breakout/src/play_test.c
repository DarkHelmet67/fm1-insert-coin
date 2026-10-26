/* SPDX-License-Identifier: GPL-3.0-only */
#include "play.h"

#include "config.h"
#include "test/check.h"

static const Paddle PADDLE = {106, 16};

static void serve_waits_for_the_cycle_then_the_ball_appears(void)
{
    Round round = round_new();
    round = round_step(&round, &PADDLE, 1, 254);
    CHECK_EQ(round.play.phase, PHASE_SERVING);
    CHECK_EQ(round.play.frames_left, 2);
    round = round_step(&round, &PADDLE, 0, 255);
    round = round_step(&round, &PADDLE, 0, 256);
    round = round_step(&round, &PADDLE, 0, 257);
    CHECK_EQ(round.play.phase, PHASE_IN_PLAY);
    CHECK_EQ(round.play.ball.y_fifths, 120 * FIFTHS);
}

static void the_last_lost_ball_ends_the_game(void)
{
    Round round = round_new();
    round.ball = BALLS_PER_GAME;
    round.play.phase = PHASE_IN_PLAY;
    round.play.ball.y_fifths = SCREEN_HEIGHT * FIFTHS - 1;
    round.play.ball.x_fifths = 10 * FIFTHS;
    round.play.ball.dir_y = 1;
    round = round_step(&round, &PADDLE, 0, 0);
    CHECK_EQ(round.play.phase, PHASE_GAME_OVER);
}

static void the_second_wall_comes_once(void)
{
    Round round = round_new();
    round.score = WALL_POINTS;
    CHECK(!round_second_wall_due(&round, 0));
    CHECK(round_second_wall_due(&round, 1));
    round.refilled = 1;
    CHECK(!round_second_wall_due(&round, 1));
}

int main(void)
{
    RUN(serve_waits_for_the_cycle_then_the_ball_appears);
    RUN(the_last_lost_ball_ends_the_game);
    RUN(the_second_wall_comes_once);
    return check_report();
}
