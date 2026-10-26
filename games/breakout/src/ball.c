/* SPDX-License-Identifier: GPL-3.0-only */
#include "ball.h"

#include "speed.h"

int ball_overlaps(const Ball *ball, int32_t x, int32_t y, int32_t width, int32_t height)
{
    /* rectsOverlap of @arcade/collision, with the rectangle in fifths too */
    return ball->x_fifths < (x + width) * FIFTHS &&
           x * FIFTHS < ball->x_fifths + BALL_WIDTH * FIFTHS &&
           ball->y_fifths < (y + height) * FIFTHS &&
           y * FIFTHS < ball->y_fifths + BALL_HEIGHT * FIFTHS;
}

Ball ball_move(const Ball *ball)
{
    const Speed speed = speed_for(ball->hits, ball->fast, ball->outer);
    Ball next = *ball;
    next.x_fifths += ball->dir_x * speed.sideways;
    next.y_fifths += ball->dir_y * speed.vertical;
    return next;
}

Ball ball_bounce_off_walls(const Ball *ball)
{
    Ball next = *ball;
    if (ball->x_fifths <= SIDE_WALL_WIDTH * FIFTHS) {
        next.x_fifths = SIDE_WALL_WIDTH * FIFTHS;
        next.dir_x = 1;
    } else if (ball->x_fifths + BALL_WIDTH * FIFTHS >= RIGHT_WALL_X * FIFTHS) {
        next.x_fifths = (RIGHT_WALL_X - BALL_WIDTH) * FIFTHS;
        next.dir_x = -1;
    } else if (ball->y_fifths <= TOP_WALL_HEIGHT * FIFTHS) {
        next.y_fifths = TOP_WALL_HEIGHT * FIFTHS;
        next.dir_y = 1;
        next.can_hit_brick = 1;
    }
    return next;
}

int ball_touches_top_wall(const Ball *ball)
{
    return ball->y_fifths <= TOP_WALL_HEIGHT * FIFTHS;
}

int32_t ball_paddle_segment(const Ball *ball, const Paddle *paddle)
{
    /* floor(((x + BALL_WIDTH / 2 - paddle.x) * 4) / width), all in fifths */
    const int32_t offset = ball->x_fifths + BALL_WIDTH * FIFTHS / 2 - paddle->x * FIFTHS;
    if (offset < 0)
        return 0; /* (C division rounds towards zero: below 0 is the first segment anyway) */
    const int32_t segment = offset * 4 / (paddle->width * FIFTHS);
    return segment > 3 ? 3 : segment;
}

Ball ball_bounce_off_paddle(const Ball *ball, const Paddle *paddle)
{
    if (ball->dir_y != 1 || !ball_overlaps(ball, paddle->x, PADDLE_Y, paddle->width, PADDLE_HEIGHT))
        return *ball;
    const int32_t segment = ball_paddle_segment(ball, paddle);
    Ball next = *ball;
    next.y_fifths = (PADDLE_Y - BALL_HEIGHT) * FIFTHS;
    next.dir_x = segment < 2 ? -1 : 1;
    next.dir_y = -1;
    next.outer = segment == 0 || segment == 3;
    next.hits = ball->hits + 1u;
    next.can_hit_brick = 1;
    return next;
}

int ball_is_lost(const Ball *ball)
{
    return ball->y_fifths >= SCREEN_HEIGHT * FIFTHS;
}

int32_t ball_pixel(int32_t fifths)
{
    /* Math.round: a fifth is never exactly half a pixel, so the nearest is never in doubt */
    return (fifths + FIFTHS / 2) / FIFTHS;
}
