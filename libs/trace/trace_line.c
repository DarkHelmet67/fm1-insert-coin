/* SPDX-License-Identifier: GPL-3.0-only */
#include "trace_line.h"

/** The value of a hex digit, or -1. */
static int32_t hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

/** Reads a decimal number at *p, moving past it; returns 0 when there is no digit. */
static int read_decimal(const char **p, uint32_t *out)
{
    if (**p < '0' || **p > '9')
        return 0;
    uint32_t value = 0;
    while (**p >= '0' && **p <= '9')
        value = value * 10u + (uint32_t)(*(*p)++ - '0');
    *out = value;
    return 1;
}

/** Reads exactly eight hex digits at p. */
static int read_hash(const char *p, uint32_t *out)
{
    uint32_t value = 0;
    for (int i = 0; i < 8; i++) {
        const int32_t digit = hex_digit(p[i]);
        if (digit < 0)
            return 0;
        value = value << 4 | (uint32_t)digit;
    }
    *out = value;
    return 1;
}

int trace_parse_line(const char *line, TraceLine *out)
{
    const char *p = line;
    TraceLine parsed;
    if (!read_decimal(&p, &parsed.frame) || *p++ != ' ')
        return 0;
    if (p[0] != 'L' && p[0] != 'R' && p[0] != '-')
        return 0;
    parsed.direction = p[0] == 'L' ? -1 : p[0] == 'R' ? 1 : 0;
    parsed.fire = p[1] == 'F';
    parsed.coin = p[2] == 'C';
    if ((p[1] != 'F' && p[1] != '-') || (p[2] != 'C' && p[2] != '-') || p[3] != ' ')
        return 0;
    parsed.screen = p[4];
    if ((p[4] != 'A' && p[4] != 'P' && p[4] != 'G') || p[5] != ' ' ||
        !read_hash(p + 6, &parsed.hash))
        return 0;
    *out = parsed;
    return 1;
}
