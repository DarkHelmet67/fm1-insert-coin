/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The serve (serve.ts): when and where the ball appears after SERVE.
 */
#ifndef BREAKOUT_SERVE_H
#define BREAKOUT_SERVE_H

#include <stdint.h>

#include "ball.h"
#include "cartridge/private.h"

/**
 * Frames between SERVE and the ball showing up. In the circuit the ball keeps circling,
 * invisible, and appears when it next crosses the middle of the screen: from at once to about
 * 4 seconds later, depending on when the button is pressed.
 * @param clock the frame counter
 */
GAME_PRIVATE uint32_t serve_delay(uint32_t clock);

/**
 * The ball as it appears after a serve, going down at the slowest speed. Its position and side
 * were whatever the hidden ball had at that moment: here they come from the frame counter at the
 * press, just as unpredictable.
 * @param pressed_at the frame counter when SERVE was pressed
 */
GAME_PRIVATE Ball serve_ball(uint32_t pressed_at);

#endif
