/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * A game as the launcher sees it: a cartridge. The launcher (launcher/) shows the menu and runs
 * one cartridge at a time; it knows nothing about Space Invaders, Breakout or Galaga, only this
 * interface.
 *
 * Every game keeps its own state, of its own type, in bytes the launcher lends it: the functions
 * receive it as `void *` and the game's cartridge.c turns it back into its own type. That file
 * is the only glue; the rest of the game is the same C as when it ran on its own.
 */
#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

#include "audio/sound.h"
#include "cartridge/private.h"
#include "input/fm1_panel.h"
#include "screen/framebuffer.h"
#include "screen/screen_map.h"

/** What the synth's panel did since the previous frame. */
typedef struct {
    uint32_t notes;                   /**< the note keys held now, bits of `fm1_in.notes` */
    uint32_t note_presses;            /**< the note keys pressed since the previous frame */
    uint32_t buttons;                 /**< the buttons held now, bits of `fm1_in.buttons` */
    uint32_t button_presses;          /**< the buttons pressed since the previous frame */
    int32_t turns[FM1_ENCODER_COUNT]; /**< encoder clicks since the previous frame, + clockwise */
} Panel;

/** The most bytes of state a game may ask for. */
#define CARTRIDGE_STATE_MAX 16384u

/** One game, as the launcher runs it. */
typedef struct {
    const char *name;    /**< in the menu, in capitals */
    uint32_t record_tag; /**< four letters that mark its best score in the flash */
    uint32_t state_size; /**< sizeof its state, at most CARTRIDGE_STATE_MAX */
    uint16_t width;      /**< its screen, in pixels */
    uint16_t height;
    /** Where its screen lands on the display. */
    ScreenMap (*screen_map)(void);
    /** Power-on: the attract screen, with the best score saved. */
    void (*start)(void *state, int32_t hi_score);
    /** One 60 Hz frame. */
    void (*step)(void *state, const Panel *panel);
    /** Draws the whole frame from scratch. */
    void (*render)(const void *state, Framebuffer *fb);
    /** The colours of its screen now (FB_COLORS of them; a game may switch film and monitor). */
    const uint16_t *(*colors)(const void *state);
    /** The sounds the last frame started, as bits: bit n plays sound_effects[n]. */
    uint32_t (*sounds)(const void *state);
    /** Its sound effects. */
    const SoundEffect *sound_effects;
    /** Its best score now. */
    int32_t (*hi_score)(const void *state);
    /** Whether a game is being played (PLAY blinks when not, a new record is saved when not). */
    int (*playing)(const void *state);
    /** The note keys it reads, to light them. */
    uint32_t (*keys)(void);
    /** The buttons it reads besides PLAY, to light them. */
    uint32_t buttons;
} Cartridge;

#endif
