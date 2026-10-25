/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"

#include "attract.h"

GameState game_initial(int32_t hi_score)
{
    GameState state;
    state.round = round_new();
    state.attract = 1;
    state.mono = 0;
    state.paddle = paddle_attract();
    state.clock = 0;
    state.hi_score = hi_score;
    state.record_to_beat = 0;
    state.ticks = ticks_none();
    return state;
}

/** A new game, started by SERVE in the attract mode: the ball waits for another SERVE. */
static GameState start_game(const GameState *state)
{
    GameState next = *state;
    next.round = round_new();
    next.attract = 0;
    next.paddle = paddle_initial();
    next.record_to_beat = state->hi_score;
    next.ticks = ticks_none();
    return next;
}

/** Back to the attract mode after the last ball, with the last game's score on show. */
static GameState end_game(const GameState *state)
{
    GameState next = *state;
    next.attract = 1;
    next.round.play.phase = PHASE_READY;
    next.paddle = paddle_attract();
    return next;
}

/** One frame of a game: the paddle, the ball, the record and the brick ticks. */
static GameState update_playing(const GameState *state, const Controls *controls)
{
    Paddle sized = state->paddle;
    sized.width = paddle_width(state->round.shrunk);
    const Paddle paddle = paddle_move(&sized, controls);
    GameState next = *state;
    next.round = round_step(&state->round, &paddle, controls->serve, state->clock);
    next.paddle = paddle;
    if (next.round.score > state->hi_score)
        next.hi_score = next.round.score;
    next.ticks = ticks_step(&state->ticks, next.round.score - state->round.score);
    return next.round.play.phase == PHASE_GAME_OVER ? end_game(&next) : next;
}

/** One frame of the attract mode, or of a game. */
static GameState update_screen(const GameState *state, const Controls *controls)
{
    if (!state->attract)
        return update_playing(state, controls);
    if (controls->serve)
        return start_game(state);
    GameState next = *state;
    next.round = attract_step(&state->round, state->clock);
    return next;
}

GameState game_step(const GameState *state, const Controls *controls)
{
    GameState next = update_screen(state, controls);
    next.clock = state->clock + 1u;
    if (controls->color_mode)
        next.mono = !state->mono;
    return next;
}
