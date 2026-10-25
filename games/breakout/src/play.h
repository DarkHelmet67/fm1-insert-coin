/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * What happens on the playfield (play.ts): the ball waiting, being served, in play, or the game
 * over; the wall, the score, the ball number.
 */
#ifndef BREAKOUT_PLAY_H
#define BREAKOUT_PLAY_H

#include <stdint.h>

#include "ball.h"
#include "bricks.h"
#include "cartridge/private.h"
#include "paddle.h"

/** The phases of play. */
typedef enum { PHASE_READY, PHASE_SERVING, PHASE_IN_PLAY, PHASE_GAME_OVER } Phase;

/** What is happening: the phase, and what it needs. */
typedef struct {
    uint8_t phase;        /**< a Phase */
    uint32_t frames_left; /**< serving: frames before the ball appears */
    uint32_t pressed_at;  /**< serving: the frame counter when SERVE was pressed */
    Ball ball;            /**< in play */
} Play;

/** The part of the game that changes during play. */
typedef struct {
    Play play;
    Wall wall;
    int32_t score;
    uint32_t ball; /**< the ball in play: 1 to BALLS_PER_GAME */
    uint8_t
        shrunk; /**< the paddle is half as wide: the ball touched the top wall since the serve */
    uint8_t refilled; /**< the second wall has been given: there is no third */
} Round;

/** A new game: full wall, no points, the first ball waiting for SERVE. */
GAME_PRIVATE Round round_new(void);

/**
 * Whether the second wall appears now. The circuit does not count the bricks left: it waits for
 * the first paddle hit once the score reaches the value of a whole wall, and only once.
 */
GAME_PRIVATE int round_second_wall_due(const Round *round, int hit_paddle);

/**
 * Advances the playfield by one frame.
 * @param round  the playfield
 * @param paddle the paddle, already moved
 * @param serve  SERVE pressed
 * @param clock  the frame counter: it decides when and where the served ball appears
 */
GAME_PRIVATE Round round_step(const Round *round, const Paddle *paddle, int serve, uint32_t clock);

#endif
