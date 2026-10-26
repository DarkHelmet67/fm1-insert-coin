/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The firmware in the browser simulator: the launcher's console (the menu and the games) with the
 * browser in place of the FM-1. The page (sim/sim.js) sends the keys, buttons and encoders, shows
 * the 240 x 240 screen and plays the sound; the code is the same that runs on the synth.
 */
#include "../../sim/sim.h"

#include "audio/mixer.h"
#include "audio/wsg.h"
#include "cartridges.h"
#include "console.h"
#include "screen/screen_push.h"

static Console console;
static Records saved;
static Framebuffer frame;
static Framebuffer spare;
static Mixer mixer;
static Wsg wsg;
static WsgFrame voices;
static const uint8_t *wsg_prom;
static int32_t turns[FM1_ENCODER_COUNT];
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

/** A best score kept in the browser, given back before sim_init(). */
SIM_EXPORT(sim_set_record) void sim_set_record(uint32_t tag, int32_t score)
{
    if (saved.version != RECORDS_VERSION)
        saved = records_empty();
    saved = records_set(&saved, tag, score);
}

/** Power-on: the menu. */
SIM_EXPORT(sim_init) void sim_init(void)
{
    if (saved.version != RECORDS_VERSION)
        saved = records_empty();
    console_start(&console, CARTRIDGES, CARTRIDGE_COUNT, &saved);
    mixer = mixer_silent();
    wsg = wsg_new();
    voices = wsg_silent_frame();
}

/** Clicks of an encoder since the last frame, + clockwise. */
SIM_EXPORT(sim_turn) void sim_turn(uint32_t encoder, int32_t clicks)
{
    if (encoder < FM1_ENCODER_COUNT)
        turns[encoder] += clicks;
}

/** One 60 Hz frame: keys and buttons held, and pressed since the last frame. */
SIM_EXPORT(sim_frame)
void sim_frame(uint32_t notes, uint32_t note_presses, uint32_t buttons, uint32_t button_presses)
{
    Panel panel = {notes, note_presses, buttons, button_presses, {0}};
    for (uint32_t e = 0; e < FM1_ENCODER_COUNT; e++) {
        panel.turns[e] = turns[e];
        turns[e] = 0;
    }
    console_step(&console, &panel);
    const Cartridge *cartridge = console_cartridge(&console);
    for (uint32_t name = 0; name < 32u; name++)
        if (console.sounds >> name & 1u)
            mixer_play(&mixer, &cartridge->sound_effects[name]);
    /* the sound chip's voices of this frame, as audio_voices() on the synth */
    const WsgFrame *frame = console_voices(&console);
    voices = frame != 0 ? *frame : wsg_silent_frame();
    wsg_prom = cartridge->wsg_prom;
    wsg_start_frame(&wsg);
}

/** Draws the frame and returns the 240 x 240 RGBA pixels of the display. */
SIM_EXPORT(sim_screen) const uint32_t *sim_screen(void)
{
    uint16_t colors[FB_COLORS];
    uint16_t row[SIM_SCREEN];
    const ScreenMap map = console_screen_map(&console);
    console_colors(&console, colors);
    fb_init(&frame, console_width(&console), console_height(&console));
    console_render(&console, &frame, &spare);
    for (int32_t y = 0; y < SIM_SCREEN; y++) {
        /* black at the sides of a narrower screen, as screen_clear() leaves them */
        for (int32_t x = 0; x < SIM_SCREEN; x++)
            row[x] = 0;
        screen_render_row(&frame, &map, y, colors, row + map.dest_x);
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
    if (wsg_prom != 0)
        wsg_render(&wsg, &voices, wsg_prom, samples, n);
    return samples;
}

/** The keys in use, as bits of the notes: their LEDs glow. */
SIM_EXPORT(sim_keys) uint32_t sim_keys(void)
{
    return console_keys(&console);
}

/** The buttons in use besides PLAY, as bits of the buttons. */
SIM_EXPORT(sim_buttons) uint32_t sim_buttons(void)
{
    return console_buttons(&console);
}

/** Whether PLAY blinks. */
SIM_EXPORT(sim_waiting) int sim_waiting(void)
{
    return console_waiting(&console);
}

/** Whether the best scores changed since the last call: then the page saves them. */
SIM_EXPORT(sim_records_changed) int sim_records_changed(void)
{
    if (records_equal(&console.records, &saved))
        return 0;
    saved = console.records;
    return 1;
}

/** The best scores, one at a time: how many, and each one's tag and score. */
SIM_EXPORT(sim_record_count) uint32_t sim_record_count(void)
{
    return saved.count;
}

/** The tag of best score `i`. */
SIM_EXPORT(sim_record_tag) uint32_t sim_record_tag(uint32_t i)
{
    return i < saved.count ? saved.list[i].tag : 0u;
}

/** Best score `i`. */
SIM_EXPORT(sim_record_score) int32_t sim_record_score(uint32_t i)
{
    return i < saved.count ? saved.list[i].score : 0;
}
