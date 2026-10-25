/* SPDX-License-Identifier: GPL-3.0-only */
#include "z80_math.h"

int32_t z80_divide(int32_t dividend, int32_t divisor)
{
    int32_t a = 0;
    int32_t hl = dividend & 0xffff;
    int32_t carry = 0;
    for (int round = 0; round < 17; round++) {
        a = a * 2 + carry;
        if (a > 0xff) {
            a = (a - divisor) & 0xff;
            carry = 1;
        } else if (a >= divisor) {
            a -= divisor;
            carry = 1;
        } else {
            carry = 0;
        }
        hl = hl * 2 + carry;
        carry = hl > 0xffff;
        hl &= 0xffff;
    }
    return hl;
}

int32_t signed8(int32_t byte)
{
    return ((byte & 0xff) ^ 0x80) - 0x80;
}

int32_t angle_to(int32_t to_y, int32_t to_x, int32_t from_y, int32_t from_x)
{
    const int32_t dx = (to_x - from_x) & 0xff;
    const int x_negative = to_x < from_x;
    const int32_t abs_x = x_negative ? (0x100 - dx) & 0xff : dx;
    const int32_t dy = (to_y - from_y) & 0xff;
    const int y_negative = to_y < from_y;
    const int32_t abs_y = y_negative ? (0x100 - dy) & 0xff : dy;
    const int32_t quadrant = y_negative ? (x_negative ^ 1) | 2 : x_negative;
    /* the "cp c" compares |dy| with |dx|; together with the quadrant it picks the half octant */
    const int32_t y_below_x = abs_y < abs_x;
    const int32_t octant = ((quadrant << 1) | (((y_below_x ^ quadrant) & 1) ? 0 : 1)) & 0xff;
    const int32_t small = y_below_x ? abs_y : abs_x;
    const int32_t large = y_below_x ? abs_x : abs_y;
    const int32_t ratio = z80_divide(small << 8, large);
    const int32_t high = ratio >> 8;
    const int32_t low = ((high ^ octant) & 1) ? ~ratio & 0xff : ratio & 0xff;
    return (((octant << 8) | low) >> 1) & 0x3ff;
}
