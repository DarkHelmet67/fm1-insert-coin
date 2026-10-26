/* SPDX-License-Identifier: GPL-3.0-only */
#include "badges.h"

/** A flag: the character it starts at and how many columns it takes. */
typedef struct {
    uint8_t first;
    uint8_t columns;
} Flag;

/** The flags of each value (character codes from the ROM, `c_build_token_2`). */
static const Flag ONE = {0x36, 1};
static const Flag FIVE = {0x38, 1};
static const Flag TEN = {0x3a, 2};
static const Flag TWENTY = {0x3e, 2};
static const Flag THIRTY = {0x42, 2};
static const Flag FIFTY = {0x46, 2};

/** The flags of `stage`, from the leftmost, into `out`; returns how many. */
static uint32_t stage_flags(int32_t stage, Flag out[MAX_FLAGS])
{
    uint32_t n = 0;
    const int32_t rest = stage % 50;
    for (int32_t i = 0; i < stage / 50 && n < MAX_FLAGS; i++)
        out[n++] = FIFTY;
    /* 40 is a 30 and a 10 */
    switch (rest / 10) {
    case 1:
        out[n++] = TEN;
        break;
    case 2:
        out[n++] = TWENTY;
        break;
    case 3:
        out[n++] = THIRTY;
        break;
    case 4:
        out[n++] = THIRTY;
        out[n++] = TEN;
        break;
    default:
        break;
    }
    if (rest % 10 >= 5)
        out[n++] = FIVE;
    for (int32_t i = 0; i < rest % 10 % 5 && n < MAX_FLAGS; i++)
        out[n++] = ONE;
    return n;
}

uint32_t stage_flag_count(int32_t stage)
{
    Flag flags[MAX_FLAGS];
    return stage_flags(stage, flags);
}

uint32_t badge_columns(int32_t stage, uint32_t shown, BadgeColumn *out)
{
    Flag flags[MAX_FLAGS];
    const uint32_t count = stage_flags(stage, flags);
    uint32_t n = 0;
    for (uint32_t f = 0; f < count && f < shown; f++) {
        for (uint32_t i = 0; i < flags[f].columns; i++) {
            const uint32_t top = flags[f].first + i * 2u;
            /* 1, 5 and 50 use palette 1, the others palette 2 */
            const BadgeColumn column = {(uint8_t)top, (uint8_t)(top + 1u),
                                        (uint8_t)(((top + 2u) & 0x0cu) == 8u ? 1 : 2)};
            out[n++] = column;
        }
    }
    return n;
}
