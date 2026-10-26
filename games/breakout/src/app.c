/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"

#include "config.h"
#include "sounds.h"

App app_initial(int32_t hi_score)
{
    App app;
    app.game = game_initial(hi_score);
    app.sounds = 0;
    return app;
}

Controls app_controls(const GameState *game, const Panel *panel)
{
    int32_t clicks = 0;
    for (uint32_t e = 0; e < FM1_ENCODER_COUNT; e++)
        clicks += panel->turns[e];
    Controls controls;
    controls.direction = 0;
    controls.has_pointer = clicks != 0;
    controls.pointer_x = game->paddle.x + game->paddle.width / 2 + clicks * KNOB_PIXELS;
    controls.serve = (panel->button_presses >> SERVE_BUTTON & 1u) != 0u;
    controls.color_mode = (panel->button_presses >> COLOR_BUTTON & 1u) != 0u;
    return controls;
}

App app_step(const App *app, const Panel *panel)
{
    const Controls controls = app_controls(&app->game, panel);
    App next;
    next.game = game_step(&app->game, &controls);
    next.sounds = sounds_for(&app->game, &next.game);
    return next;
}
