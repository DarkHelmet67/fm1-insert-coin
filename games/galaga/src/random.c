/* SPDX-License-Identifier: GPL-3.0-only */
#include "random.h"

int32_t random_byte(int32_t seed, int32_t n)
{
    /* Math.imul and >>> in 32 bits: unsigned arithmetic wraps the same way */
    const uint32_t x = (uint32_t)seed * 0x9e3779b9u + (uint32_t)n;
    const uint32_t a = (x ^ (x >> 16)) * 0x7feb352du;
    const uint32_t b = (a ^ (a >> 15)) * 0x846ca68bu;
    return (int32_t)((b ^ (b >> 16)) & 0xffu);
}
