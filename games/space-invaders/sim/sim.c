/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Space Invaders in the browser simulator: the firmware's frame (app_step, render_game, the
 * sounds) with the browser in place of the FM-1. The page (sim/sim.js) sends the keys, shows the
 * 240 x 240 screen and plays the sound; the game code is the same that runs on the synth.
 */
#include "../../../sim/sim.h"

#include "app.h"
#include "audio/mixer.h"
#include "config.h"
#include "render.h"
#include "screen/screen_push.h"
#include "sounds.h"

static App app;
static Framebuffer frame;
static ScreenMap map;
static Mixer mixer;
static uint32_t rgba[SIM_SCREEN * SIM_SCREEN];
static int32_t samples[SIM_AUDIO_MAX];

/** An RGB565 colour as the page's RGBA bytes (little endian: R, G, B, A in memory). */
static uint32_t rgba_of(uint16_t c)
{
    const uint32_t r = (c >> 11) * 255u / 31u;
    const uint32_t g = (c >> 5 & 63u) * 255u / 63u;
    const uint32_t b = (c & 31u) * 255u / 31u;
    return 0xFF000000u | b << 16 | g << 8 | r;
}

/** Power-on, with the record saved in the browser. */
SIM_EXPORT(sim_init) void sim_init(int32_t hi_score)
{
    app = app_initial(hi_score);
    map = screen_map_rows(GAME_WIDTH, SHOWN_ROWS, 2u);
    mixer = mixer_silent();
    fb_init(&frame, GAME_WIDTH, GAME_HEIGHT);
}

/** One 60 Hz frame: keys held, keys and buttons pressed since the last frame. */
SIM_EXPORT(sim_frame) void sim_frame(uint32_t notes, uint32_t note_presses, uint32_t button_presses)
{
    app = app_step(&app, notes, note_presses, button_presses);
    for (uint32_t name = 0; name < SOUND_COUNT; name++)
        if (app.sounds & SOUND_BIT(name))
            mixer_play(&mixer, &SOUND_EFFECTS[name]);
}

/** Draws the frame and returns the 240 x 240 RGBA pixels of the display. */
SIM_EXPORT(sim_screen) const uint32_t *sim_screen(void)
{
    uint16_t row[SIM_SCREEN];
    const uint16_t *palette = app.mono ? MONO_COLORS : COLORS;
    render_game(&frame, &app.game);
    for (int32_t y = 0; y < SIM_SCREEN; y++) {
        /* the game is narrower than the display: background at the sides, as screen_clear() */
        for (int32_t x = 0; x < SIM_SCREEN; x++)
            row[x] = palette[COLOR_BACKGROUND];
        screen_render_row(&frame, &map, y, palette, row + map.dest_x);
        for (int32_t x = 0; x < SIM_SCREEN; x++)
            rgba[y * SIM_SCREEN + x] = rgba_of(row[x]);
    }
    return rgba;
}

/** The next `n` samples of sound, one channel at 44.1 kHz, Q15. */
SIM_EXPORT(sim_audio) const int32_t *sim_audio(uint32_t n)
{
    if (n > SIM_AUDIO_MAX)
        n = SIM_AUDIO_MAX;
    for (uint32_t i = 0; i < n; i++)
        samples[i] = 0;
    mixer_render(&mixer, samples, n);
    return samples;
}

/** The keys the game uses, as bits of the notes: their LEDs glow. */
SIM_EXPORT(sim_game_keys) uint32_t sim_game_keys(void)
{
    return keymap_keys(&CONTROLS);
}

/** The button that starts a game, and the one that switches the colours. */
SIM_EXPORT(sim_coin_button) uint32_t sim_coin_button(void)
{
    return COIN_BUTTON;
}

SIM_EXPORT(sim_color_button) uint32_t sim_color_button(void)
{
    return COLOR_BUTTON;
}

/** Whether PLAY can start a game now (its LED blinks). */
SIM_EXPORT(sim_waiting) int sim_waiting(void)
{
    return app.game.screen != SCREEN_PLAYING;
}

/** The record to save, or -1 when there is nothing new to save. */
SIM_EXPORT(sim_record_to_save) int32_t sim_record_to_save(int32_t saved)
{
    return app_record_to_save(&app, saved) ? app.game.hi_score : -1;
}
