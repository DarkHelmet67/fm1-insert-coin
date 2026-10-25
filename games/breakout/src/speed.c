/* SPDX-License-Identifier: GPL-3.0-only */
#include "speed.h"

#include "config.h"

Speed speed_for(uint32_t hits, int fast, int outer)
{
    Speed speed;
    if (fast) {
        speed.vertical = FAST_VERTICAL;
        speed.sideways = FAST_SIDEWAYS;
        return speed;
    }
    const SpeedRow *row = &BALL_SPEEDS[0];
    for (uint32_t i = 1; i < SPEED_ROWS; i++)
        if (hits >= BALL_SPEEDS[i].from_hits)
            row = &BALL_SPEEDS[i];
    speed.vertical = row->vertical;
    speed.sideways = outer ? row->outer : row->middle;
    return speed;
}
