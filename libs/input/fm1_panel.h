/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The FM-1's buttons and encoders by name, as ids of `fm1_in.buttons` and of fm1_enc_take(): the
 * same numbers as the BTN_ and ENC_ names of Dinghy's platform.c, for the code that does not
 * include it (the games, the launcher, the simulator).
 */
#ifndef FM1_PANEL_H
#define FM1_PANEL_H

/** The bit of button `b` in `fm1_in.buttons`. */
#define FM1_BUTTON_BIT(b) (1u << (b))

/** The buttons the games use. */
enum {
    FM1_BUTTON_FX = 2,
    FM1_BUTTON_PLAY = 12,
};

/** The seven encoders: the three on the left of the screen, then the four knobs. */
enum {
    FM1_ENCODER_SELECT = 0,
    FM1_ENCODER_ALGORITHM = 1,
    FM1_ENCODER_KNOB1 = 2,
    FM1_ENCODER_KNOB2 = 3,
    FM1_ENCODER_KNOB3 = 4,
    FM1_ENCODER_KNOB4 = 5,
    FM1_ENCODER_PRESETS = 6,
};

/** How many encoders the FM-1 has. */
#define FM1_ENCODER_COUNT 7u

#endif
