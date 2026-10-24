/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"

GameState game_initial(int32_t hi_score)
{
    GameState state;
    state.screen = SCREEN_ATTRACT;
    state.playing = playing_initial(1);
    state.record_to_beat = hi_score;
    state.hi_score = hi_score;
    state.frames_left = 0;
    state.attract_frames = 0;
    return state;
}

/** A new game, keeping the best score. */
static GameState start_game(const GameState *state)
{
    GameState next = *state;
    next.screen = SCREEN_PLAYING;
    next.playing = playing_initial(1);
    next.record_to_beat = state->hi_score;
    return next;
}

/** One frame of play: the game ends when playing_is_over says so, updating the best score. */
static GameState step_playing(const GameState *state, const Controls *controls)
{
    GameState next = *state;
    next.playing = playing_step(&state->playing, controls);
    if (next.playing.score > next.hi_score)
        next.hi_score = next.playing.score;
    if (playing_is_over(&next.playing)) {
        next.screen = SCREEN_GAME_OVER;
        next.frames_left = GAME_OVER_FRAMES;
    }
    return next;
}

/** One frame of the game over screen, then back to the attract screen. */
static GameState step_game_over(const GameState *state)
{
    GameState next = *state;
    if (state->frames_left > 1u) {
        next.frames_left--;
        return next;
    }
    next.screen = SCREEN_ATTRACT;
    next.attract_frames = 0;
    return next;
}

GameState game_step(const GameState *state, const Controls *controls)
{
    if (controls->coin && state->screen != SCREEN_PLAYING)
        return start_game(state);
    switch (state->screen) {
    case SCREEN_PLAYING:
        return step_playing(state, controls);
    case SCREEN_GAME_OVER:
        return step_game_over(state);
    default: {
        GameState next = *state;
        next.attract_frames++;
        return next;
    }
    }
}
