/* SPDX-License-Identifier: GPL-3.0-only */
#include "test/check.h"
#include "z80_math.h"

static void it_divides_16_bits_by_8_rounding_down(void)
{
    CHECK_EQ(z80_divide(0x1000, 0x10), 0x100);
    CHECK_EQ(z80_divide(100, 7), 14);
    CHECK_EQ(z80_divide(0xffff, 1), 0xffff);
}

static void dividing_by_zero_gives_ffff_as_the_z80_routine(void)
{
    CHECK_EQ(z80_divide(1234, 0), 0xffff);
}

static void signed8_reads_a_byte_as_signed(void)
{
    CHECK_EQ(signed8(0xff), -1);
    CHECK_EQ(signed8(0x7f), 127);
    CHECK_EQ(signed8(0x180), -128);
}

/* Odd octants come out one step short (up is $FF, not $100), exactly as in the original. */
static void angles_point_right_up_left_and_down(void)
{
    CHECK_EQ(angle_to(0x40, 0x50, 0x40, 0x40), 0x000);
    CHECK_EQ(angle_to(0x50, 0x40, 0x40, 0x40), 0x0ff);
    CHECK_EQ(angle_to(0x40, 0x30, 0x40, 0x40), 0x1ff);
    CHECK_EQ(angle_to(0x30, 0x40, 0x40, 0x40), 0x300);
}

static void angles_point_along_the_diagonals(void)
{
    CHECK_EQ(angle_to(0x50, 0x50, 0x40, 0x40), 0x080);
    CHECK_EQ(angle_to(0x30, 0x30, 0x40, 0x40), 0x280);
    CHECK_EQ(angle_to(0x50, 0x30, 0x40, 0x40), 0x17f);
    CHECK_EQ(angle_to(0x30, 0x50, 0x40, 0x40), 0x37f);
}

static void the_angle_grows_with_the_slope_inside_an_octant(void)
{
    const int32_t shallow = angle_to(0x44, 0x50, 0x40, 0x40);
    const int32_t steeper = angle_to(0x48, 0x50, 0x40, 0x40);
    CHECK(shallow > 0);
    CHECK(steeper > shallow);
    CHECK(steeper < 0x80);
}

int main(void)
{
    RUN(it_divides_16_bits_by_8_rounding_down);
    RUN(dividing_by_zero_gives_ffff_as_the_z80_routine);
    RUN(signed8_reads_a_byte_as_signed);
    RUN(angles_point_right_up_left_and_down);
    RUN(angles_point_along_the_diagonals);
    RUN(the_angle_grows_with_the_slope_inside_an_octant);
    return check_report();
}
