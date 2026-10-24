/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Space Invaders on the M-Vave FM-1: the firmware's entry points.
 *
 * This file is the only one that talks to the hardware. It includes Dinghy's platform.c (boot,
 * USB, updater, keys, screen, sound) and supplies the two functions the platform calls:
 * app_main(), the main loop, and app_audio(), the sound interrupt. Everything else in src/ and
 * libs/ is plain C with no hardware in it, so it is tested on a computer.
 *
 * Every frame (60 Hz): read the keys, advance the cabinet (app_step: the game, its sounds, its
 * colours), draw the whole frame from scratch into `back` (render_game), copy the rows that
 * changed into `front` (fb_present) and send only those to the display (screen_push). The sound
 * interrupt mixes the sounds asked for (audio_render). The browser simulator (sim/) runs the
 * same app_step and render_game.
 */
#define APP_NAME "INVADERS"     /* the rescue and the crash screens */
#define STORE_MAGIC 0x494E5644u /* "INVD": the saved record is ours, not another firmware's */
#include "platform.c"

#include "app.h"
#include "audio/audio_fm1.h"
#include "clock/frame_clock.h"
#include "config.h"
#include "render.h"
#include "screen/screen_fm1.h"
#include "sounds.h"
#include "storage/hi_score_fm1.h"

/** The most frames the loop catches up at once, after the main loop was held up (a flash save). */
#define MAX_CATCH_UP 4u

/** What the display shows, and the frame being drawn: about 70 KB each, in the memory pool. */
static Framebuffer front __attribute__((section(".pool")));
static Framebuffer back __attribute__((section(".pool")));

/** The cabinet: about 1 KB, kept off the stack of a loop that calls deeper functions. */
static App app;

/**
 * Lights the game keys: dim when idle, bright while held. PLAY blinks while it can start a game.
 * @param notes the keys held now
 */
static void light_keys(uint32_t notes)
{
    const uint32_t used = keymap_keys(&CONTROLS);
    for (uint32_t k = 0; k < FM1_KEY_COUNT; k++) {
        if (!(used & FM1_KEY_BIT(k)))
            continue;
        fm1_led_key_glow(KEY_LED(k), 1);
        fm1_led_key(KEY_LED(k), (notes & FM1_KEY_BIT(k)) != 0u);
    }
    const int waiting = app.game.screen != SCREEN_PLAYING;
    fm1_led_key_glow(COIN_BUTTON, 1);
    fm1_led_key(COIN_BUTTON, waiting && fm1_ms % 1000u < 500u);
    fm1_led_key_glow(COLOR_BUTTON, 1);
}

static void app_audio(int32_t *out, uint32_t n)
{
    audio_render(out, n);
}

/** Starts the sounds of a frame. */
static void play_sounds(uint32_t mask)
{
    for (uint32_t name = 0; name < SOUND_COUNT; name++)
        if (mask & SOUND_BIT(name))
            audio_play(&SOUND_EFFECTS[name]);
}

static void app_main(void)
{
    const ScreenMap map = screen_map_rows(GAME_WIDTH, SHOWN_ROWS, 2u);
    uint16_t color_palette[FB_COLORS], mono_palette[FB_COLORS];
    screen_palette(COLORS, color_palette);
    screen_palette(MONO_COLORS, mono_palette);
    fb_init(&front, GAME_WIDTH, GAME_HEIGHT);
    fb_init(&back, GAME_WIDTH, GAME_HEIGHT);
    app = app_initial(hi_score_load());
    int32_t saved_hi_score = app.game.hi_score;
    uint8_t shown_mono = app.mono;
    uint32_t last_notes = 0, note_presses = 0, button_presses = 0;
    screen_clear();
    uint32_t done = frame_clock_frames(fm1_ms);
    for (;;) {
        platform_poll();
        /* presses gather until a frame uses them: a tap between two frames is not lost */
        const uint32_t notes = fm1_in.notes;
        uint32_t released;
        note_presses |= fm1_input_note_edges() | (notes & ~last_notes);
        button_presses |= fm1_input_edges(&released);
        last_notes = notes;
        const uint32_t due = frame_clock_frames(fm1_ms);
        if (due - done > MAX_CATCH_UP)
            done = due - MAX_CATCH_UP;
        if (done == due) {
            screen_push(&front, &map, shown_mono ? mono_palette : color_palette);
            continue;
        }
        for (; done != due; done++) {
            app = app_step(&app, notes, note_presses, button_presses);
            play_sounds(app.sounds);
            note_presses = button_presses = 0; /* a press counts in one frame only */
        }
        /*
         * a new record is saved after the game, when nothing sounds: erasing the flash holds up
         * the loop and the sound interrupt for about 50 ms
         */
        if (app_record_to_save(&app, saved_hi_score) && !audio_is_playing()) {
            hi_score_save(app.game.hi_score);
            saved_hi_score = app.game.hi_score;
        }
        if (app.mono != shown_mono) {
            shown_mono = app.mono;
            fb_mark_all_dirty(&front); /* same pixels, new colours: send everything again */
        }
        render_game(&back, &app.game);
        fb_present(&front, &back);
        light_keys(notes);
        screen_push(&front, &map, shown_mono ? mono_palette : color_palette);
    }
}
