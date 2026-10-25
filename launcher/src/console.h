/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The launcher at work: the menu and the cartridges, the state of the game being run, the best
 * scores. What the firmware (main.c) and the browser simulator (sim/sim.c) both run, so the
 * simulator plays exactly the firmware.
 *
 * Like main.c, this is the imperative shell around the pure parts (launcher.c, menu.c,
 * records.c and each game's own logic): it keeps the game's state in place, in bytes lent to the
 * cartridge, instead of copying 16 KB every frame.
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

#include "cartridge/cartridge.h"
#include "launcher.h"
#include "records.h"

/** Everything the launcher keeps. */
typedef struct {
    Launcher launcher;
    const Cartridge *const *cartridges;
    Records records; /**< the best scores, as they should be in the flash */
    uint32_t sounds; /**< the sounds of the last frame, bits of the cartridge's sound_effects */
    union {
        uint8_t bytes[CARTRIDGE_STATE_MAX];
        uint64_t align; /**< any game's struct can live here */
    } state;            /**< the chosen game's state */
} Console;

/**
 * Power-on: the menu, the first game chosen and its attract screen running behind the picture.
 * @param console    where everything is kept
 * @param cartridges the games, in the menu's order
 * @param count      how many (at least 1, each with a state of at most CARTRIDGE_STATE_MAX)
 * @param records    the best scores read from the flash
 */
void console_start(Console *console, const Cartridge *const *cartridges, uint32_t count,
                   const Records *records);

/** One 60 Hz frame: the menu, or the game. */
void console_step(Console *console, const Panel *panel);

/** The game chosen in the menu, or being played. */
const Cartridge *console_cartridge(const Console *console);

/** Whether the menu is on the screen. */
int console_in_menu(const Console *console);

/** The size of the screen drawn now: the menu's, or the game's. */
uint32_t console_width(const Console *console);
uint32_t console_height(const Console *console);

/** Where the screen drawn now lands on the display. */
ScreenMap console_screen_map(const Console *console);

/** The colours of the screen drawn now. */
void console_colors(const Console *console, uint16_t out[FB_COLORS]);

/**
 * Draws the whole frame from scratch.
 * @param console the launcher
 * @param screen  console_width() x console_height(): what goes to the display
 * @param game    a spare screen, for the game's frame before it shrinks into the menu
 */
void console_render(const Console *console, Framebuffer *screen, Framebuffer *game);

/**
 * What the game's sound chip plays in the last frame, or NULL: in the menu (the games are silent
 * behind its picture) and for a game without a WSG chip.
 */
const WsgFrame *console_voices(const Console *console);

/** The note keys to light: the game's, in a game. */
uint32_t console_keys(const Console *console);

/** The buttons to light besides PLAY. */
uint32_t console_buttons(const Console *console);

/** Whether PLAY blinks: in the menu, and in a game waiting for a coin. */
int console_waiting(const Console *console);

#endif
