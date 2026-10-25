/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The whole of Breakout as one file, for the firmware and the simulator: with GAME_AMALGAMATION
 * every function and table of the game is static (libs/cartridge/private.h), so its names never
 * meet those of the other games. Only BREAKOUT_CARTRIDGE is public.
 * A new module of the game goes in this list too: the link fails without it.
 */
#define GAME_AMALGAMATION

#include "src/app.c"
#include "src/attract.c"
#include "src/ball.c"
#include "src/brick_hit.c"
#include "src/bricks.c"
#include "src/cartridge.c"
#include "src/config.c"
#include "src/game.c"
#include "src/hud.c"
#include "src/paddle.c"
#include "src/play.c"
#include "src/render.c"
#include "src/serve.c"
#include "src/sounds.c"
#include "src/speed.c"
#include "src/ticks.c"
#include "src/trace_state.c"
