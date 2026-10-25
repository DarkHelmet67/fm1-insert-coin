/* SPDX-License-Identifier: GPL-3.0-only */
#include "audio/pow2.h"
#include "test/check.h"

/** Whether a is within `per_mille` thousandths of b. */
static int near(int64_t a, int64_t b, int64_t per_mille)
{
    const int64_t d = a > b ? a - b : b - a;
    return d * 1000 <= (b < 0 ? -b : b) * per_mille + 1;
}

static void powers_of_two_have_exact_logarithms(void)
{
    CHECK_EQ(log2_q16(1), 0);
    CHECK_EQ(log2_q16(2), OCTAVE_Q16);
    CHECK_EQ(log2_q16(1024), 10 * OCTAVE_Q16);
}

static void logarithms_in_between_are_close(void)
{
    CHECK(near(log2_q16(3), 103872, 1));      /* 1.585 octaves */
    CHECK(near(log2_q16(44100), 1011720, 1)); /* 15.43 */
    CHECK(near(log2_q16(3100) - log2_q16(780), 130460, 3));
}

static void scaling_by_whole_octaves_shifts(void)
{
    CHECK_EQ(scale_pow2(1000, OCTAVE_Q16), 2000u);
    CHECK_EQ(scale_pow2(1000, -2 * OCTAVE_Q16), 250u);
    CHECK_EQ(scale_pow2(1000, 0), 1000u);
}

static void scaling_by_fractions_is_close(void)
{
    CHECK(near(scale_pow2(10000, OCTAVE_Q16 / 2), 14142, 1));
    CHECK(near(scale_pow2(10000, -OCTAVE_Q16 / 4), 8409, 1));
    CHECK(near(scale_pow2(32767, -10 * OCTAVE_Q16 - OCTAVE_Q16 / 3), 25, 50));
}

int main(void)
{
    RUN(powers_of_two_have_exact_logarithms);
    RUN(logarithms_in_between_are_close);
    RUN(scaling_by_whole_octaves_shifts);
    RUN(scaling_by_fractions_is_close);
    return check_report();
}
