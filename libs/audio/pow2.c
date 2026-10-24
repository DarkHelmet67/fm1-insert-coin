/* SPDX-License-Identifier: GPL-3.0-only */
#include "pow2.h"

/** 2^(i/64) in Q14, i = 0..64: one octave in 64 steps, interpolated in between. */
static const uint16_t POW2_TABLE[65] = {
    16384, 16562, 16743, 16925, 17109, 17296, 17484, 17674, 17867, 18061, 18258, 18457, 18658,
    18861, 19066, 19274, 19484, 19696, 19911, 20127, 20347, 20568, 20792, 21019, 21247, 21479,
    21713, 21949, 22188, 22430, 22674, 22921, 23170, 23423, 23678, 23936, 24196, 24460, 24726,
    24995, 25268, 25543, 25821, 26102, 26386, 26674, 26964, 27258, 27554, 27855, 28158, 28464,
    28774, 29088, 29405, 29725, 30048, 30376, 30706, 31041, 31379, 31720, 32066, 32415, 32768,
};

/** log2(1 + i/64) in Q16, i = 0..64. */
static const uint32_t LOG2_TABLE[65] = {
    0,     1466,  2909,  4331,  5732,  7112,  8473,  9814,  11136, 12440, 13727, 14996, 16248,
    17484, 18704, 19909, 21098, 22272, 23433, 24579, 25711, 26830, 27936, 29029, 30109, 31178,
    32234, 33279, 34312, 35334, 36346, 37346, 38336, 39316, 40286, 41246, 42196, 43137, 44068,
    44990, 45904, 46809, 47705, 48593, 49472, 50344, 51207, 52063, 52911, 53751, 54584, 55410,
    56229, 57040, 57845, 58643, 59434, 60219, 60997, 61769, 62534, 63294, 64047, 64794, 65536,
};

/** Interpolates between table[i] and table[i + 1] by `frac` out of 1024. */
static uint32_t interpolate(uint32_t a, uint32_t b, uint32_t frac)
{
    return a + ((b - a) * frac >> 10);
}

int32_t log2_q16(uint32_t x)
{
    if (x == 0u)
        return 0;
    int32_t octave = 31;
    while (!(x & 0x80000000u)) {
        x <<= 1;
        octave--;
    }
    /* x is now 1.f in Q31: the 6 bits after the leading one pick the step, the next 10 interpolate
     */
    const uint32_t index = (x >> 25) & 63u;
    const uint32_t frac = (x >> 15) & 1023u;
    return octave * OCTAVE_Q16 +
           (int32_t)interpolate(LOG2_TABLE[index], LOG2_TABLE[index + 1u], frac);
}

uint32_t scale_pow2(uint32_t value, int32_t exponent_q16)
{
    /* the arithmetic shift rounds down: -0.25 octaves is -1 octave plus 0.75 */
    const int32_t octaves = exponent_q16 >> 16;
    const uint32_t fraction = (uint32_t)exponent_q16 & 0xFFFFu;
    const uint32_t index = fraction >> 10;
    const uint32_t m = interpolate(POW2_TABLE[index], POW2_TABLE[index + 1u], fraction & 1023u);
    const uint32_t scaled = value * m >> 14;
    if (octaves >= 0)
        return octaves > 14 ? 0xFFFFFFFFu : scaled << octaves;
    return octaves < -31 ? 0u : scaled >> -octaves;
}
