/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"

#include "config.h"
#include "test/check.h"

static Panel no_panel(void)
{
    const Panel panel = {0, 0, 0, 0, {0}};
    return panel;
}

static void any_encoder_turns_the_knob(void)
{
    App app = app_initial(0);
    Panel panel = no_panel();
    panel.button_presses = FM1_BUTTON_BIT(SERVE_BUTTON);
    app = app_step(&app, &panel); /* a game */
    CHECK_EQ(app.game.attract, 0);
    panel = no_panel();
    panel.turns[FM1_ENCODER_KNOB1] = 2;
    panel.turns[FM1_ENCODER_PRESETS] = 1;
    const int32_t before = app.game.paddle.x;
    app = app_step(&app, &panel);
    CHECK_EQ(app.game.paddle.x, before + 3 * KNOB_PIXELS);
    panel = no_panel();
    panel.turns[FM1_ENCODER_SELECT] = -1;
    app = app_step(&app, &panel);
    CHECK_EQ(app.game.paddle.x, before + 2 * KNOB_PIXELS);
}

static void a_still_knob_leaves_the_paddle(void)
{
    const Panel panel = no_panel();
    const App app = app_initial(0);
    CHECK_EQ(app_controls(&app.game, &panel).has_pointer, 0);
}

static void fx_switches_the_monitor(void)
{
    App app = app_initial(0);
    Panel panel = no_panel();
    panel.button_presses = FM1_BUTTON_BIT(COLOR_BUTTON);
    app = app_step(&app, &panel);
    CHECK_EQ(app.game.mono, 1);
}

int main(void)
{
    RUN(any_encoder_turns_the_knob);
    RUN(a_still_knob_leaves_the_paddle);
    RUN(fx_switches_the_monitor);
    return check_report();
}
