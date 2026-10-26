/* SPDX-License-Identifier: GPL-3.0-only */
#include "cartridges.h"

const Cartridge *const CARTRIDGES[] = {
    &SPACE_INVADERS_CARTRIDGE,
    &BREAKOUT_CARTRIDGE,
    &GALAGA_CARTRIDGE,
};

const uint32_t CARTRIDGE_COUNT = sizeof CARTRIDGES / sizeof CARTRIDGES[0];
