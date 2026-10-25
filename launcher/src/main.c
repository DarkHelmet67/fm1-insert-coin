/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * fm1-insert-coin on the M-Vave FM-1: the firmware's entry points.
 *
 * This file is the only one that talks to the hardware. It includes Dinghy's platform.c (boot,
 * USB, updater, keys, screen, sound) and supplies the two functions the platform calls:
 * app_main(), the main loop, and app_audio(), the sound interrupt. Everything else is plain C
 * with no hardware in it, so it is tested on a computer.
 *
 * Every frame (60 Hz): read the panel, advance the launcher (console_step: the menu or the game),
 * draw the whole frame from scratch into `back` (console_render), copy the rows that changed
 * into `front` (fb_present) and send only those to the display, a band at a time between frames
 * (screen_push_band). The sound interrupt mixes the sounds asked for (audio_render). The browser
 * simulator (sim/) runs the same console.
 */
#define APP_NAME "INSERT COIN" /* the rescue and the crash screens */
/*
 * "INVD": the saved records are ours, not another firmware's. The same mark as the Space
 * Invaders firmware before the menu, so its best score carries over (records_load).
 */
#define STORE_MAGIC 0x494E5644u
#include "platform.c"

#include "audio/audio_fm1.h"
#include "cartridges.h"
#include "clock/frame_clock.h"
#include "console.h"
#include "input/fm1_keys.h"
#include "screen/screen_fm1.h"

/** The most frames the loop catches up at once, after the main loop was held up (a flash save). */
#define MAX_CATCH_UP 4u

/**
 * What the display shows, the frame being drawn, and a spare one for the game's frame before it
 * shrinks into the menu's picture: about 70 KB each, in the memory pool.
 */
static Framebuffer front __attribute__((section(".pool")));
static Framebuffer back __attribute__((section(".pool")));
static Framebuffer spare __attribute__((section(".pool")));

/** The launcher and the game's state: about 17 KB, kept off the stack. */
static Console console;

/** What the Space Invaders firmware saved before the menu came: one score. */
typedef struct {
    uint32_t version;
    int32_t hi_score;
} OldRecord;

/** The best scores saved in the flash, or none; the old Space Invaders score is taken over. */
static Records records_load(void)
{
    Records records;
    OldRecord old;
    if (settings_load(&records, sizeof records) == 0 && records_valid(&records))
        return records;
    records = records_empty();
    if (settings_load(&old, sizeof old) == 0 && old.version == 1u && old.hi_score >= 0)
        return records_set(&records, SPACE_INVADERS_CARTRIDGE.record_tag, old.hi_score);
    return records;
}

/**
 * Lights the keys and buttons in use: dim when idle, bright while held. PLAY blinks while it can
 * start something.
 * @param notes the keys held now
 */
static void light_panel(uint32_t notes)
{
    const uint32_t used = console_keys(&console);
    const uint32_t buttons = console_buttons(&console);
    for (uint32_t k = 0; k < FM1_KEY_COUNT; k++) {
        const int lit = (used & FM1_KEY_BIT(k)) != 0u;
        fm1_led_key_glow(KEY_LED(k), lit);
        fm1_led_key(KEY_LED(k), lit && (notes & FM1_KEY_BIT(k)) != 0u);
    }
    fm1_led_key_glow(BTN_PLAY, 1);
    fm1_led_key(BTN_PLAY, console_waiting(&console) && fm1_ms % 1000u < 500u);
    fm1_led_key_glow(BTN_FX, (buttons & FM1_BUTTON_BIT(FM1_BUTTON_FX)) != 0u);
}

static void app_audio(int32_t *out, uint32_t n)
{
    audio_render(out, n);
}

/** Starts the sounds of a frame, and gives the sound chip's voices of the game that has one. */
static void play_sounds(void)
{
    const Cartridge *cartridge = console_cartridge(&console);
    for (uint32_t name = 0; name < 32u; name++)
        if (console.sounds >> name & 1u)
            audio_play(&cartridge->sound_effects[name]);
    audio_voices(console_voices(&console), cartridge->wsg_prom);
}

/** The panel since the last frame: presses gather until a frame uses them. */
static Panel panel;

/** Reads the keys, buttons and encoders into `panel`; a tap between two frames is not lost. */
static void read_panel(void)
{
    static uint32_t last_notes;
    uint32_t released;
    panel.notes = fm1_in.notes;
    panel.buttons = fm1_in.buttons;
    panel.note_presses |= fm1_input_note_edges() | (panel.notes & ~last_notes);
    panel.button_presses |= fm1_input_edges(&released);
    last_notes = panel.notes;
    for (uint32_t e = 0; e < FM1_ENCODER_COUNT; e++) /* FM1_ENCODER_x is Dinghy's ENC_x */
        panel.turns[e] += fm1_enc_take(e);
}

/** Forgets the presses and the clicks once a frame has used them. */
static void panel_used(void)
{
    panel.note_presses = panel.button_presses = 0;
    for (uint32_t e = 0; e < FM1_ENCODER_COUNT; e++)
        panel.turns[e] = 0;
}

/** Sets the screens up for what is shown now (the menu or a game) and clears the display. */
static ScreenMap show(void)
{
    fb_init(&front, console_width(&console), console_height(&console));
    fb_init(&back, console_width(&console), console_height(&console));
    screen_clear();
    return console_screen_map(&console);
}

/**
 * Draws the latest frame into `front`, ready to be sent; the panel's lights follow it.
 * @param shown_palette the colours on the display, updated to the game's now
 */
static void draw_frame(uint16_t shown_palette[FB_COLORS])
{
    uint16_t colors[FB_COLORS], palette[FB_COLORS];
    console_colors(&console, colors);
    screen_palette(colors, palette);
    for (uint32_t i = 0; i < FB_COLORS; i++) {
        if (palette[i] != shown_palette[i])
            fb_mark_all_dirty(&front); /* same pixels, new colours: send everything again */
        shown_palette[i] = palette[i];
    }
    console_render(&console, &back, &spare);
    fb_present(&front, &back);
    light_panel(panel.notes);
}

static void app_main(void)
{
    Records saved = records_load();
    console_start(&console, CARTRIDGES, CARTRIDGE_COUNT, &saved);
    uint16_t shown_palette[FB_COLORS] = {0};
    int shown_menu = console_in_menu(&console);
    ScreenMap map = show();
    uint32_t done = frame_clock_frames(fm1_ms);
    int drawn = 1;   /* the latest frame is drawn */
    int sending = 0; /* bands of the drawn frame may still be on their way to the display */
    for (;;) {
        platform_poll();
        read_panel();
        const uint32_t due = frame_clock_frames(fm1_ms);
        if (due - done > MAX_CATCH_UP)
            done = due - MAX_CATCH_UP;
        if (done != due) {
            for (; done != due; done++) {
                console_step(&console, &panel);
                play_sounds();
                panel_used(); /* a press counts in one frame only */
            }
            drawn = 0;
            /*
             * a new record is saved after the game, when nothing sounds: erasing the flash holds
             * up the loop and the sound interrupt for about 50 ms
             */
            if (!records_equal(&console.records, &saved) && !audio_is_playing()) {
                saved = console.records;
                (void)settings_save(&saved, sizeof saved);
            }
            if (console_in_menu(&console) != shown_menu) { /* a game is entered from the menu */
                shown_menu = console_in_menu(&console);
                map = show();
                sending = 0;
            }
        }
        /*
         * A frame is drawn once the previous one is all on the display. A screen that changes
         * everywhere (Galaga's stars) takes several frames to send: it shows whole, never torn,
         * fewer times a second, while the game and its sound keep their pace between the bands.
         */
        if (!drawn && !sending) {
            draw_frame(shown_palette);
            drawn = 1;
        }
        sending = screen_push_band(&front, &map, shown_palette);
    }
}
