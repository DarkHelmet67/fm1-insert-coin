/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Space Invaders as a cartridge of the launcher: the glue between the launcher's interface
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

/** Where the screen lands: rows 8-239 and 243-250, 1:1 (config.h). */
static ScreenMap invaders_screen_map(void)
{
    return screen_map_rows(GAME_WIDTH, SHOWN_ROWS, 2u);
}

/** Power-on: the attract screen. */
static void invaders_start(void *state, int32_t hi_score)
{
    *app_of(state) = app_initial(hi_score);
}

/** One frame of the cabinet, from the panel. */
static void invaders_step(void *state, const Panel *panel)
{
    App *app = app_of(state);
    *app = app_step(app, panel->notes, panel->note_presses, panel->button_presses);
}

/** The whole frame, from scratch. */
static void invaders_render(const void *state, Framebuffer *fb)
{
    render_game(fb, &const_app_of(state)->game);
}

/** The coloured film, or the black and white monitor after FX. */
static const uint16_t *invaders_colors(const void *state)
{
    return const_app_of(state)->mono ? MONO_COLORS : COLORS;
}

/** The sounds the last frame started. */
static uint32_t invaders_sounds(const void *state)
{
    return const_app_of(state)->sounds;
}

/** The best score now, which changes during play. */
static int32_t invaders_hi_score(const void *state)
{
    return const_app_of(state)->game.hi_score;
}

/** Whether a game is on (not the attract or game over screen). */
static int invaders_playing(const void *state)
{
    return const_app_of(state)->game.screen == SCREEN_PLAYING;
}

/** The keys that move and fire. */
static uint32_t invaders_keys(void)
{
    return keymap_keys(&CONTROLS);
}

/** The cartridge: the only name of Space Invaders the rest of the firmware sees. */
const Cartridge SPACE_INVADERS_CARTRIDGE = {
    .name = "SPACE INVADERS",
    .record_tag = 0x494E5644u, /* "INVD", as the record of the Space Invaders firmware */
    .state_size = sizeof(App),
    .width = GAME_WIDTH,
    .height = GAME_HEIGHT,
    .screen_map = invaders_screen_map,
    .start = invaders_start,
    .step = invaders_step,
    .render = invaders_render,
    .colors = invaders_colors,
    .sounds = invaders_sounds,
    .sound_effects = SOUND_EFFECTS,
    .hi_score = invaders_hi_score,
    .playing = invaders_playing,
    .keys = invaders_keys,
    .buttons = FM1_BUTTON_BIT(COLOR_BUTTON),
};
