/* SPDX-License-Identifier: GPL-3.0-only */
#include "ticks.h"

#include "config.h"

Ticks ticks_none(void)
{
    const Ticks ticks = {0, 0u, 0u};
    return ticks;
}

Ticks ticks_step(const Ticks *ticks, int32_t points)
{
    Ticks next = *ticks;
    next.owed = ticks->owed + points;
    if (ticks->wait > 0u) {
        next.wait = ticks->wait - 1u;
        return next;
    }
    if (next.owed == 0)
        return *ticks;
    next.owed--;
    next.played = ticks->played + 1u;
    next.wait = TICK_FRAMES - 1u;
    return next;
}
