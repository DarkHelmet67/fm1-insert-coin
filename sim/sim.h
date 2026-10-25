/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What every game's simulator exports to the browser page (sim/sim.js), the same for all games:
 * the page knows nothing about the game, only the FM-1's keys, buttons, screen and sound.
 */
#ifndef SIM_H
#define SIM_H

#include <stdint.h>

/** Makes a function callable from JavaScript under its own name. */
#define SIM_EXPORT(name) __attribute__((export_name(#name)))

/** Display size, and how many samples sim_audio() fills at most. */
#define SIM_SCREEN 240
#define SIM_AUDIO_MAX 4096

#endif
