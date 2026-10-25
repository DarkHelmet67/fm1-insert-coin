/* SPDX-License-Identifier: GPL-3.0-only */
#include "clock.h"

int32_t slow_clock(int32_t frame)
{
    return frame <= 0 ? 0 : (frame >> 5) * 2 + ((frame & 0x1f) >= 1 ? 1 : 0);
}

int timers_tick(int32_t frame)
{
    return frame > 0 && (frame & 0x1f) == 0;
}

GameTimers no_timers(void)
{
    const GameTimers timers = {{0, 0, 0, 0}};
    return timers;
}

GameTimers tick_timers(const GameTimers *timers, int32_t frame)
{
    GameTimers next = *timers;
    if (!timers_tick(frame))
        return next;
    for (int i = 0; i < 4; i++)
        next.t[i] = timers->t[i] > 1 ? timers->t[i] - 1 : 0;
    return next;
}

GameTimers set_timer(const GameTimers *timers, int index, int32_t value)
{
    GameTimers next = *timers;
    next.t[index] = value;
    return next;
}
