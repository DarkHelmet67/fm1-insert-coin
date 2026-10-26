/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The values of the FM-1 port that are a choice rather than a fact of the original, in one place
 * so they are easy to change and to test (as tuning.config.ts in the web version).
 */
#ifndef GALAGA_CONFIG_H
#define GALAGA_CONFIG_H

#include "audio/sound.h"
#include "cartridge/private.h"
#include "input/keymap.h"
#include "screen/screen_map.h"
#include "stage_setup.h"

/**
 * The difficulty (DIP switches): A is the factory setting, the easiest; B, C and D make the
 * aliens dive and bomb more.
 */
#define GAME_RANK RANK_A

/** Fighters in reserve at the start: 3 fighters in all, the factory setting. */
#define START_RESERVE 2

/**
 * Extra fighters, in tens of thousands of points: the first at 20 000, the second at 70 000,
 * then one every 70 000, the factory setting.
 */
#define EXTRA_LIFE_FIRST 2
#define EXTRA_LIFE_EVERY 7

/**
 * The controls, the same as Space Invaders (Luca, 2026-10-25): OP1-OP3 move left, OP4-OP5 fire,
 * OP6, PIT and GLO move right.
 */
GAME_EXTERN const KeyMap CONTROLS;

/** PLAY inserts the coin and starts a game (an id of fm1_in.buttons, platform/docs/API.md). */
#define COIN_BUTTON 12u /* BTN_PLAY */

/**
 * The 288 rows of the original screen on the 240 of the display (Luca, 2026-10-25: "squeeze it
 * vertically"): of every 6 rows the last is left out, 288 / 6 * 5 = 240. The columns stay 1:1.
 * SQUEEZE_RANGES ranges of 5 rows each.
 */
#define SQUEEZE_RANGES 48u
GAME_EXTERN const RowRange SHOWN_ROWS[SQUEEZE_RANGES];

/**
 * The fighter's explosion, which on the cabinet is a noise of the 54XX chip: a filtered noise
 * fading in about a second, the web version's approximation (tuning.config.ts, `explosion`).
 * Sound effect 0 of the cartridge.
 */
GAME_EXTERN const SoundEffect SOUND_EFFECTS[1];

#endif
