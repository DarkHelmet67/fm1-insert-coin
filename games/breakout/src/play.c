/* SPDX-License-Identifier: GPL-3.0-only */
#include "play.h"

#include "brick_hit.h"
#include "config.h"
#include "serve.h"

/** A play in one phase, its other fields empty. */
static Play play_in(Phase phase)
{
    Play play;
    play.phase = (uint8_t)phase;
    play.frames_left = 0;
    play.pressed_at = 0;
    play.ball = serve_ball(0); /* unused outside PHASE_IN_PLAY: any ball */
    return play;
}

Round round_new(void)
{
    Round round;
    round.play = play_in(PHASE_READY);
    round.wall = wall_full();
    round.score = 0;
    round.ball = 1;
    round.shrunk = 0;
    round.refilled = 0;
    return round;
}

/** After a lost ball: the next one, or the end of the game after the last. */
static Round lose_ball(const Round *round)
{
    Round next = *round;
    next.shrunk = 0;
    if (round->ball < BALLS_PER_GAME) {
        next.play = play_in(PHASE_READY);
        next.ball = round->ball + 1u;
    } else {
        next.play = play_in(PHASE_GAME_OVER);
    }
    return next;
}

int round_second_wall_due(const Round *round, int hit_paddle)
{
    return hit_paddle && !round->refilled && round->score >= WALL_POINTS;
}

/**
 * One frame of the ball in play: move, bounce, break a brick, or get lost. Touching the top
 * wall halves the paddle; hitting the paddle with a whole wall's worth of points brings the
 * bricks back.
 */
static Round update_ball(const Round *round, const Ball *ball, const Paddle *paddle)
{
    const Ball moved = ball_move(ball);
    const Ball walled = ball_bounce_off_walls(&moved);
    const Ball bounced = ball_bounce_off_paddle(&walled, paddle);
    /* the paddle sends a falling ball back up: that is how a hit shows */
    const int hit_paddle = walled.dir_y == 1 && bounced.dir_y == -1;
    const int refill = round_second_wall_due(round, hit_paddle);
    const Wall full = wall_full();
    const BrickHit hit = brick_hit(&bounced, refill ? &full : &round->wall);
    Round next = *round;
    next.wall = hit.wall;
    next.score = round->score + hit.points;
    next.shrunk = round->shrunk || ball_touches_top_wall(&moved);
    next.refilled = round->refilled || refill;
    if (ball_is_lost(&hit.ball))
        return lose_ball(&next);
    next.play = play_in(PHASE_IN_PLAY);
    next.play.ball = hit.ball;
    return next;
}

Round round_step(const Round *round, const Paddle *paddle, int serve, uint32_t clock)
{
    Round next = *round;
    switch (round->play.phase) {
    case PHASE_READY:
        if (serve) {
            next.play = play_in(PHASE_SERVING);
            next.play.frames_left = serve_delay(clock);
            next.play.pressed_at = clock;
        }
        return next;
    case PHASE_SERVING:
        if (round->play.frames_left > 0u) {
            next.play.frames_left--;
        } else {
            next.play = play_in(PHASE_IN_PLAY);
            next.play.ball = serve_ball(round->play.pressed_at);
        }
        return next;
    case PHASE_IN_PLAY:
        return update_ball(round, &round->play.ball, paddle);
    default:
        return next; /* the game over: the game decides what comes next (game.c) */
    }
}
