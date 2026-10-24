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
 * Step 2: a test card at the original resolution, drawn through the shared screen library: the
 * playfield's edges, the ground line, the reserve row, and a block the game keys move at 60 Hz.
 */
#define APP_NAME "INVADERS" /* the rescue and the crash screens */
#include "platform.c"

#include "clock/frame_clock.h"
#include "config.h"
#include "screen/screen_fm1.h"

/** The most frames the loop catches up at once, after the main loop was held up (a flash save). */
#define MAX_CATCH_UP 4u

/** The game's screen, in the memory pool (about 70 KB). */
static Framebuffer screen __attribute__((section(".pool")));

/** Where the block is, and its size. */
static int32_t block_x = 8;
#define BLOCK_Y 216
#define BLOCK_W 13
#define BLOCK_H 8

/** Draws what does not move: the playfield's corners, the ground line, the reserve row. */
static void draw_test_card(Framebuffer *fb)
{
    fb_fill_rect(fb, 0, 8, GAME_WIDTH, 1, COLOR_TEXT);       /* first shown row */
    fb_fill_rect(fb, 0, 8, 1, 232, COLOR_TEXT);              /* left edge */
    fb_fill_rect(fb, GAME_WIDTH - 1, 8, 1, 232, COLOR_TEXT); /* right edge */
    fb_fill_rect(fb, 0, 239, GAME_WIDTH, 1, COLOR_CANNON);   /* ground line */
    for (int32_t i = 0; i < 3; i++)                          /* reserve cannons */
        fb_fill_rect(fb, 24 + i * 16, 243, BLOCK_W, BLOCK_H, COLOR_CANNON);
    for (int32_t i = 0; i < 5; i++) /* the invader colours, one bar each */
        fb_fill_rect(fb, 40 + i * 30, 64, 16, 8, (uint8_t)(COLOR_UFO + i));
}

/**
 * One frame: the block moves one pixel a frame, as the cannon does.
 * @param actions ACTION_* bits
 */
static void step(uint32_t actions)
{
    const int32_t dx = (actions & ACTION_LEFT) ? -1 : (actions & ACTION_RIGHT) ? 1 : 0;
    const int32_t x = block_x + dx;
    if (x < 1 || x + BLOCK_W > (int32_t)GAME_WIDTH - 1)
        return;
    const uint8_t color = (actions & ACTION_FIRE) ? COLOR_TEXT : COLOR_CANNON;
    fb_fill_rect(&screen, block_x, BLOCK_Y, BLOCK_W, BLOCK_H, COLOR_BACKGROUND);
    fb_fill_rect(&screen, x, BLOCK_Y, BLOCK_W, BLOCK_H, color);
    block_x = x;
}

/**
 * Lights the game keys: dim when idle, bright while held.
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
}

static void app_audio(int32_t *out, uint32_t n)
{
    (void)out; /* silent for now: out is already zero */
    (void)n;
}

static void app_main(void)
{
    const ScreenMap map = screen_map_rows(GAME_WIDTH, SHOWN_ROWS, 2u);
    uint16_t palette[FB_COLORS];
    screen_palette(COLORS, palette);
    fb_init(&screen, GAME_WIDTH, GAME_HEIGHT);
    draw_test_card(&screen);
    step(0u);
    screen_clear();
    uint32_t done = frame_clock_frames(fm1_ms);
    for (;;) {
        platform_poll();
        const uint32_t due = frame_clock_frames(fm1_ms);
        if (due - done > MAX_CATCH_UP)
            done = due - MAX_CATCH_UP;
        const uint32_t notes = fm1_in.notes;
        for (; done != due; done++)
            step(keymap_actions(&CONTROLS, notes));
        light_keys(notes);
        screen_push(&screen, &map, palette);
    }
}
