/* SPDX-License-Identifier: GPL-3.0-only */
#include "ufo.h"

/** Points of the ship, indexed by the shots fired modulo 15. */
static const int16_t UFO_SCORES[15] = {100, 50,  50,  100, 150, 100, 100, 50,
                                       300, 100, 100, 100, 50,  150, 100};

Ufo ufo_initial(void)
{
    Ufo ufo = {UFO_INTERVAL, 0, 1, 0, 0};
    return ufo;
}

int32_t ufo_score(uint32_t shots_fired)
{
    return UFO_SCORES[shots_fired % 15u];
}

/** A new ship: the parity of the shot count decides which side it enters from. */
static Ufo launch(uint32_t shots_fired)
{
    Ufo ufo = {UFO_INTERVAL, 1, 1, UFO_LEFT_X, 0};
    if (shots_fired % 2u) {
        ufo.direction = -1;
        ufo.x = UFO_RIGHT_X;
    }
    return ufo;
}

Ufo ufo_step(const Ufo *ufo, uint32_t aliens_left, uint32_t shots_fired)
{
    Ufo next = *ufo;
    if (ufo->flying) {
        next.x = (int16_t)(ufo->x + ufo->direction);
        next.age++;
        if (next.x < UFO_LEFT_X || next.x > UFO_RIGHT_X)
            next = ufo_shot_down(ufo);
        return next;
    }
    if (ufo->countdown > 0) {
        next.countdown--;
        return next;
    }
    if (aliens_left >= UFO_MIN_ALIENS)
        return launch(shots_fired);
    next.countdown = UFO_INTERVAL;
    return next;
}

Ufo ufo_shot_down(const Ufo *ufo)
{
    Ufo next = *ufo;
    next.flying = 0;
    next.x = 0;
    next.direction = 1;
    next.age = 0;
    return next;
}
