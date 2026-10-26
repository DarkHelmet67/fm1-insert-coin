/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Breakout as a cartridge of the launcher: the glue between the launcher's interface
 * (libs/cartridge/cartridge.h) and the game's own functions. The state the launcher lends is an
 * App (app.h).
 */
#include "cartridge/cartridge.h"

#include "app.h"
#include "config.h"
#include "render.h"
#include "sounds.h"

/** The launcher's bytes as the game's state. */
static App *app_of(void *state)
{
    return (App *)state;
}

/** The launcher's bytes as the game's state, to read. */
static const App *const_app_of(const void *state)
{
    return (const App *)state;
}

/** The 228 x 208 screen in the middle of the display, 1:1. */
static ScreenMap breakout_screen_map(void)
{
    return screen_map_centered(SCREEN_WIDTH, SCREEN_HEIGHT);
}

/** Power-on: the attract mode. */
static void breakout_start(void *state, int32_t hi_score)
{
    *app_of(state) = app_initial(hi_score);
}

/** One frame of the cabinet, from the panel. */
static void breakout_step(void *state, const Panel *panel)
{
    App *app = app_of(state);
    *app = app_step(app, panel);
}

/** The whole frame, from scratch. */
static void breakout_render(const void *state, Framebuffer *fb)
{
    render_game(fb, &const_app_of(state)->game);
}

/** The coloured film, or the black and white monitor after FX. */
static const uint16_t *breakout_colors(const void *state)
{
    return const_app_of(state)->game.mono ? MONO_COLORS : COLORS;
}

/** The sounds the last frame started. */
static uint32_t breakout_sounds(const void *state)
{
    return const_app_of(state)->sounds;
}

/** The best score now, which changes during play. */
static int32_t breakout_hi_score(const void *state)
{
    return const_app_of(state)->game.hi_score;
}

/** Whether a game is on (not the attract mode). */
static int breakout_playing(const void *state)
{
    return !const_app_of(state)->game.attract;
}

/** No note keys: the encoders play. */
static uint32_t breakout_keys(void)
{
    return 0;
}

/** The cartridge: the only name of Breakout the rest of the firmware sees. */
const Cartridge BREAKOUT_CARTRIDGE = {
    .name = "BREAKOUT",
    .record_tag = 0x42524B54u, /* "BRKT" */
    .state_size = sizeof(App),
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
    .screen_map = breakout_screen_map,
    .start = breakout_start,
    .step = breakout_step,
    .render = breakout_render,
    .colors = breakout_colors,
    .sounds = breakout_sounds,
    .sound_effects = SOUND_EFFECTS,
    .hi_score = breakout_hi_score,
    .playing = breakout_playing,
    .keys = breakout_keys,
    .buttons = FM1_BUTTON_BIT(COLOR_BUTTON),
};
