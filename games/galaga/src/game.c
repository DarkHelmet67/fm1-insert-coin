/* SPDX-License-Identifier: GPL-3.0-only */
#include "game.h"

#include "program.h"
#include "update_playing.h"

void create_game(GameState *state, int32_t hi_score)
{
    state->screen = SCREEN_ATTRACT;
    state->hi_score = hi_score;
    new_attract(&state->on.attract);
}

void new_game(PlayingState *playing)
{
    empty_playing_state(playing, STEP_INTRO);
    playing->stage = 0;
    start_intro(playing);
}

void update_game(GameState *state, const Controls *controls)
{
    if (state->screen == SCREEN_ATTRACT) {
        if (controls->coin) {
            state->screen = SCREEN_PLAYING;
            new_game(&state->on.playing);
        } else {
            update_attract(&state->on.attract);
        }
        return;
    }
    update_playing(&state->on.playing, controls);
    if (state->on.playing.score > state->hi_score)
        state->hi_score = state->on.playing.score;
    if (state->on.playing.phase.step == STEP_OVER)
        create_game(state, state->hi_score);
}
