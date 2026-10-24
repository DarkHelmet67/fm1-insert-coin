/* SPDX-License-Identifier: GPL-3.0-only */
#include "frame_clock.h"

uint32_t frame_clock_frames(uint32_t ms)
{
    /* ms * 60 / 1000 = ms * 3 / 50, split so the product never overflows 32 bits */
    return ms / 50u * 3u + ms % 50u * 3u / 50u;
}
