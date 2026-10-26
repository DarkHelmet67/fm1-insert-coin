/* SPDX-License-Identifier: GPL-3.0-only */
#include "sounds.h"
#include "test/check.h"

/** A game on, with a ball in play at (x, y) pixels going down and right. */
static GameState playing(int32_t x, int32_t y)
{
    GameState state = game_initial(0);
    state.attract = 0;
    state.round.play.phase = PHASE_IN_PLAY;
    state.round.play.ball.x_fifths = x * FIFTHS;
    state.round.play.ball.y_fifths = y * FIFTHS;
    state.round.play.ball.dir_x = 1;
    state.round.play.ball.dir_y = 1;
    return state;
}

static void the_paddle_and_the_walls_beep(void)
{
    const GameState before = playing(100, 180);
    GameState after = before;
    after.round.play.ball.dir_y = -1;
    after.round.play.ball.y_fifths = (PADDLE_Y - BALL_HEIGHT) * FIFTHS;
    CHECK_EQ(sounds_for(&before, &after), SOUND_BIT(SOUND_PADDLE));
    after = before;
    after.round.play.ball.dir_x = -1;
    after.round.play.ball.x_fifths = (RIGHT_WALL_X - BALL_WIDTH) * FIFTHS;
    CHECK_EQ(sounds_for(&before, &after), SOUND_BIT(SOUND_WALL));
}

static void every_brick_tick_beeps(void)
{
    const GameState before = playing(100, 100);
    GameState after = before;
    after.ticks.played = 1;
    CHECK_EQ(sounds_for(&before, &after), SOUND_BIT(SOUND_BRICK));
}

static void beating_the_record_plays_the_fanfare_once(void)
{
    GameState before = playing(100, 100);
    before.record_to_beat = 10;
    before.round.score = 10;
    GameState after = before;
    after.round.score = 11;
    CHECK(sounds_for(&before, &after) & SOUND_BIT(SOUND_NEW_RECORD));
    before.round.score = 11;
    after.round.score = 12;
    CHECK(!(sounds_for(&before, &after) & SOUND_BIT(SOUND_NEW_RECORD)));
}

static void the_attract_mode_is_silent(void)
{
    const GameState before = game_initial(0);
    GameState after = before;
    after.ticks.played = 3;
    CHECK_EQ(sounds_for(&before, &after), 0);
}

int main(void)
{
    RUN(the_paddle_and_the_walls_beep);
    RUN(every_brick_tick_beeps);
    RUN(beating_the_record_plays_the_fanfare_once);
    RUN(the_attract_mode_is_silent);
    return check_report();
}
