/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The whole of Space Invaders as one file, for the firmware and the simulator: with
 * GAME_AMALGAMATION every function and table of the game is static (libs/cartridge/private.h),
 * so its names never meet those of the other games. Only SPACE_INVADERS_CARTRIDGE is public.
 * A new module of the game goes in this list too: the link fails without it.
 */
#define GAME_AMALGAMATION

#include "src/aliens.c"
#include "src/app.c"
#include "src/bombs.c"
#include "src/cannon.c"
#include "src/cartridge.c"
#include "src/collisions.c"
#include "src/config.c"
#include "src/effects.c"
#include "src/fleet.c"
#include "src/game.c"
#include "src/input.c"
#include "src/playing.c"
#include "src/render.c"
#include "src/shields.c"
#include "src/sounds.c"
#include "src/sprites.c"
#include "src/trace_state.c"
#include "src/ufo.c"
