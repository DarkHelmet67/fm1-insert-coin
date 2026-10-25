/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"

#include "config.h"
#include "input.h"
#include "sounds.h"

App app_initial(int32_t hi_score)
{
    App app;
    app.game = game_initial(hi_score);
    app.mono = 0;
    app.sounds = 0;
    return app;
}

App app_step(const App *app, uint32_t notes, uint32_t note_presses, uint32_t button_presses)
{
    const Controls controls =
        input_controls(&CONTROLS, notes, note_presses, button_presses, COIN_BUTTON);
    App next = *app;
    next.game = game_step(&app->game, &controls);
    next.sounds = sounds_for(&app->game, &next.game);
    if (button_presses >> COLOR_BUTTON & 1u)
        next.mono = !app->mono;
    return next;
}

int app_record_to_save(const App *app, int32_t saved_hi_score)
{
    return app->game.screen != SCREEN_PLAYING && app->game.hi_score > saved_hi_score;
}
