/* SPDX-License-Identifier: GPL-3.0-only */
#include "bomber_timers.h"

#include "rom.h"
#include "table.h"

/**
 * `c_08BE`: a byte of the 4-per-row table, the column given by the tens of aliens left. The
 * column is not clamped, as in the original: with all 40 aliens alive it is 4, and the program
 * reads the first byte of the next row (the commented source notes that the two tables must be
 * contiguous for this). The ROM bytes after the table are copied too, so the port reads the same
 * values.
 */
static int32_t by_aliens_left(int32_t first, int32_t row, int32_t alive)
{
    return TABLE(BOMB_ENABLE_TABLE, first + row * 4 + alive / 10);
}

/** `c_08AD`: a byte of a 3-per-row table, shorter as time passes. */
static int32_t by_time(const uint8_t *table, uint32_t size, int32_t row, int32_t timer)
{
    return table_at(table, size, row * 3 + (timer < 0x28) + (timer == 0), 0);
}

BomberSettings bomber_settings(const StageParameters *params, int32_t alive, int32_t timer,
                               int continuous, int32_t max_bombers)
{
    BomberSettings settings;
    if (continuous) {
        settings.reload[0] = settings.reload[1] = settings.reload[2] = 2;
    } else {
        settings.reload[0] = by_aliens_left(32, params->boss_timer, alive);
        settings.reload[1] =
            by_time(RED_BOMBER_TIMERS, RED_BOMBER_TIMERS_SIZE, params->red_timer, timer);
        settings.reload[2] =
            by_time(YELLOW_BOMBER_TIMERS, YELLOW_BOMBER_TIMERS_SIZE, params->yellow_timer, timer);
    }
    settings.bomb_bits = by_aliens_left(0, params->bomb_enable, alive);
    settings.max_bombers = timer < 0x3c ? params->max_bombers_later : max_bombers;
    settings.continuous = continuous;
    return settings;
}

int continuous_bombing(const StageParameters *params, int32_t alive, int fire_enabled)
{
    return alive < params->continuous_bombing && fire_enabled;
}
