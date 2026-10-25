/* SPDX-License-Identifier: GPL-3.0-only */
#include "score.h"

#include "rom.h"
#include "table.h"

int32_t points_of(int32_t color)
{
    int32_t step = TABLE(SCORE_STEPS, 15 - color);
    return (step & 0x0f) * 10 + (step >> 4) * 100;
}

int32_t hit_points(const int32_t counts[HIT_COUNTERS])
{
    int32_t sum = 0;
    for (int32_t color = 0; color < HIT_COUNTERS; color++)
        sum += counts[color] * points_of(color);
    return sum;
}
