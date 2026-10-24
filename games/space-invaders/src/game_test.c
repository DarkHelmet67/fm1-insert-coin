/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"
#include "test/check.h"

static const Controls IDLE = {0, 0, 0};
static const Controls COIN = {0, 0, 1};
static const Controls RIGHT = {1, 0, 0};

/** GameState is about 1 KB: the tests keep theirs out of the stack. */
static GameState game;

static void it_waits_on_the_attract_screen_until_a_coin_is_inserted(void)
{
    game = game_initial(0);
    game = game_step(&game, &IDLE);
    CHECK_EQ(game.screen, SCREEN_ATTRACT);
    CHECK_EQ(game.attract_frames, 1u);
    game = game_step(&game, &COIN);
    CHECK_EQ(game.screen, SCREEN_PLAYING);
}

static void it_remembers_the_record_to_beat_for_the_whole_game(void)
{
    game = game_initial(500);
    game = game_step(&game, &COIN);
    game.playing.score = 600;
    game = game_step(&game, &IDLE);
    CHECK_EQ(game.hi_score, 600);
    CHECK_EQ(game.record_to_beat, 500);
}

static void it_moves_the_cannon_while_playing(void)
{
    game = game_initial(0);
    game = game_step(&game, &COIN);
    game = game_step(&game, &RIGHT);
    CHECK_EQ(game.playing.cannon_x, CANNON_MIN_X + 1);
}

static void it_shows_game_over_and_keeps_the_best_score(void)
{
    game = game_initial(100);
    game = game_step(&game, &COIN);
    game.playing.lives = 0;
    game.playing.score = 250;
    game = game_step(&game, &IDLE);
    CHECK_EQ(game.screen, SCREEN_GAME_OVER);
    CHECK_EQ(game.frames_left, GAME_OVER_FRAMES);
    CHECK_EQ(game.hi_score, 250);
}

static void it_goes_back_to_the_attract_screen_after_the_message(void)
{
    game = game_initial(0);
    game.screen = SCREEN_GAME_OVER;
    game.frames_left = 2;
    game = game_step(&game, &IDLE);
    CHECK_EQ(game.screen, SCREEN_GAME_OVER);
    game = game_step(&game, &IDLE);
    CHECK_EQ(game.screen, SCREEN_ATTRACT);
}

static void it_accepts_a_coin_during_the_game_over_message(void)
{
    game = game_initial(0);
    game.screen = SCREEN_GAME_OVER;
    game.frames_left = 100;
    game = game_step(&game, &COIN);
    CHECK_EQ(game.screen, SCREEN_PLAYING);
    CHECK_EQ(game.playing.lives, STARTING_LIVES);
}

int main(void)
{
    RUN(it_waits_on_the_attract_screen_until_a_coin_is_inserted);
    RUN(it_remembers_the_record_to_beat_for_the_whole_game);
    RUN(it_moves_the_cannon_while_playing);
    RUN(it_shows_game_over_and_keeps_the_best_score);
    RUN(it_goes_back_to_the_attract_screen_after_the_message);
    RUN(it_accepts_a_coin_during_the_game_over_message);
    return check_report();
}
