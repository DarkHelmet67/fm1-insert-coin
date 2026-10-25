/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The games of the firmware, in the menu's order. Each one is its game's unity.c, compiled on
 * its own (games/<game>/unity.c).
 */
#ifndef CARTRIDGES_H
#define CARTRIDGES_H

#include <stdint.h>

#include "cartridge/cartridge.h"

/** Each game's cartridge, from its cartridge.c. */
extern const Cartridge SPACE_INVADERS_CARTRIDGE;

/** The games in the menu's order. */
extern const Cartridge *const CARTRIDGES[];

/** How many games. */
extern const uint32_t CARTRIDGE_COUNT;

#endif
