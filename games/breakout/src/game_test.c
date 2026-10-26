/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"
#include "test/check.h"

static Controls press(int serve)
{
    const Controls controls = {0, 0, 0, (uint8_t)serve, 0};
    return controls;
}

static void serve_starts_a_game_with_the_record_to_beat(void)
{
    const GameState start = game_initial(120);
    const Controls serve = press(1);
    const GameState next = game_step(&start, &serve);
    CHECK_EQ(next.attract, 0);
    CHECK_EQ(next.record_to_beat, 120);
    CHECK_EQ(next.paddle.width, 16);
    CHECK_EQ(next.clock, 1);
}

static void the_attract_mode_plays_by_itself(void)
{
    GameState state = game_initial(0);
    const Controls none = press(0);
    for (int i = 0; i < 400; i++)
        state = game_step(&state, &none);
    CHECK_EQ(state.attract, 1);
    CHECK_EQ(state.round.play.phase, PHASE_IN_PLAY);
    CHECK_EQ(state.round.score, 0);
}

static void the_colour_switch_toggles_the_monitor(void)
{
    const GameState start = game_initial(0);
    Controls controls = press(0);
    controls.color_mode = 1;
    CHECK_EQ(game_step(&start, &controls).mono, 1);
}

int main(void)
{
    RUN(serve_starts_a_game_with_the_record_to_beat);
    RUN(the_attract_mode_plays_by_itself);
    RUN(the_colour_switch_toggles_the_monitor);
    return check_report();
}
