/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Galaga as a cartridge of the launcher: the glue between the launcher's interface
 * (libs/cartridge/cartridge.h) and the game's own functions. The state the launcher lends is an
 * App (app.h).
 */
#include "cartridge/cartridge.h"

#include "app.h"
#include "config.h"
#include "playfield.h"
#include "render.h"
#include "rom.h"

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

/** Where the screen lands: 5 rows of every 6, the columns 1:1 (config.h). */
static ScreenMap galaga_screen_map(void)
{
    return screen_map_rows(SCREEN_WIDTH, SHOWN_ROWS, SQUEEZE_RANGES);
}

/** Power-on: the attract screen. */
static void galaga_start(void *state, int32_t hi_score)
{
    app_start(app_of(state), hi_score);
}

/** One frame of the cabinet, from the panel. */
static void galaga_step(void *state, const Panel *panel)
{
    app_step(app_of(state), panel->notes, panel->note_presses, panel->button_presses);
}

/** The whole frame, from scratch. */
static void galaga_render(const void *state, Framebuffer *fb)
{
    render_game(fb, &const_app_of(state)->game);
}

/** The colours of the colour PROMs. */
static const uint16_t *galaga_colors_of(const void *state)
{
    return const_app_of(state)->colors;
}

/** The sound effects the last frame started: the explosion. */
static uint32_t galaga_sounds(const void *state)
{
    return const_app_of(state)->sounds;
}

/** What the WSG chip plays in the last frame. */
static const WsgFrame *galaga_voices(const void *state)
{
    return &const_app_of(state)->voices;
}

/** The best score now, which changes during play. */
static int32_t galaga_hi_score(const void *state)
{
    return const_app_of(state)->game.hi_score;
}

/** Whether a game is on. */
static int galaga_playing(const void *state)
{
    return const_app_of(state)->game.screen == SCREEN_PLAYING;
}

/** The keys that move and fire. */
static uint32_t galaga_keys(void)
{
    return keymap_keys(&CONTROLS);
}

/** The cartridge: the only name of Galaga the rest of the firmware sees. */
const Cartridge GALAGA_CARTRIDGE = {
    .name = "GALAGA",
    .record_tag = 0x474C4741u, /* "GLGA" */
    .state_size = sizeof(App),
    .width = SCREEN_WIDTH,
    .height = SCREEN_HEIGHT,
    .screen_map = galaga_screen_map,
    .start = galaga_start,
    .step = galaga_step,
    .render = galaga_render,
    .colors = galaga_colors_of,
    .sounds = galaga_sounds,
    .sound_effects = SOUND_EFFECTS,
    .voices = galaga_voices,
    .wsg_prom = WAVEFORM_PROM,
    .hi_score = galaga_hi_score,
    .playing = galaga_playing,
    .keys = galaga_keys,
    .buttons = 0u,
};
