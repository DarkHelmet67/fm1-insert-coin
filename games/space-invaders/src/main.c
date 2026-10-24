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
 * Step 1: the title, and the eight game keys lit; a key held shows its action.
 */
#define APP_NAME "INVADERS" /* the rescue and the crash screens */
#include "platform.c"

#include "config.h"

/** The arcade green of the web version's cannon. */
#define C_GREEN RGB(0x20, 0xFF, 0x20)

/** Draws what never changes: the title and the key legend. */
static void draw_title(void)
{
    lcd_fill(0, 0, 240, 240, C_BLACK);
    draw_text_box(0, 48, 240, &FONT_L, "SPACE", C_WHITE, 1);
    draw_text_box(0, 84, 240, &FONT_L, "INVADERS", C_WHITE, 1);
    draw_text_box(0, 132, 240, &FONT_S, "FM-1 EDITION", C_GREEN, 1);
    draw_text_box(0, 216, 240, &FONT_S, "OP1-3  OP4-5  OP6-GLO", C_GRAY, 1);
}

/**
 * Shows the actions asked for now, in the line above the legend.
 * @param actions ACTION_* bits
 */
static void draw_actions(uint32_t actions)
{
    char text[] = "  <   FIRE   >  ";
    if (!(actions & ACTION_LEFT))
        text[2] = ' ';
    if (!(actions & ACTION_FIRE))
        memcpy(&text[6], "    ", 4);
    if (!(actions & ACTION_RIGHT))
        text[13] = ' ';
    draw_text_box(0, 184, 240, &FONT_S, text, C_GREEN, 1);
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
    uint32_t shown = 0xFFFFFFFFu; /* nothing drawn yet: the first pass draws */
    palette_set(0);
    draw_title();
    for (;;) {
        platform_poll();
        const uint32_t notes = fm1_in.notes;
        const uint32_t actions = keymap_actions(&CONTROLS, notes);
        light_keys(notes);
        if (actions != shown) {
            draw_actions(actions);
            shown = actions;
        }
    }
}
