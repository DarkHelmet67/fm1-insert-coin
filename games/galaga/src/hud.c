/* SPDX-License-Identifier: GPL-3.0-only */
#include "hud.h"

#include "badges.h"
#include "messages.h"
#include "palette.h"
#include "text.h"
#include "tiles.h"

/** Character palettes of the texts, as the program sets them before each string. */
#define TEXT_WHITE 3
#define TEXT_RED 4

/** Rows of the bottom status lines. */
#define BOTTOM_ROW 34

/** At most 5 reserve fighters are shown, two columns each. */
#define RESERVE_SHOWN 5

/** The bottom rows keep the palette written at power-up (`j_Game_init`). */
#define BOTTOM_PALETTE 3

/** Characters of a reserve fighter: top left, top right, bottom left, bottom right. */
static const uint8_t RESERVE_CHARS[4] = {0x4a, 0x4b, 0x4c, 0x4d};

char *score_text(int32_t score, char out[7])
{
    char digits[12];
    decimal(score % 1000000, digits);
    int32_t length = 0;
    while (digits[length] != '\0')
        length++;
    for (int32_t i = 0; i < 6; i++)
        out[i] = ' ';
    out[6] = '\0';
    if (length < 2) {
        out[4] = '0';
        out[5] = digits[0];
        return out;
    }
    for (int32_t i = 0; i < length; i++)
        out[6 - length + i] = digits[i];
    return out;
}

void draw_scores(Framebuffer *fb, int32_t score, int32_t hi_score, int blink_off)
{
    char text[7];
    if (!blink_off)
        draw_cell_text(fb, "1UP", 2, 0, TEXT_RED);
    draw_cell_text(fb, "HIGH SCORE", 9, 0, TEXT_RED);
    draw_cell_text(fb, score_text(score, text), 0, 1, TEXT_WHITE);
    draw_cell_text(fb, score_text(hi_score, text), 11, 1, TEXT_WHITE);
}

void draw_badges(Framebuffer *fb, int32_t stage, int32_t shown)
{
    BadgeColumn columns[2 * MAX_FLAGS];
    uint32_t all = badge_columns(stage, stage_flag_count(stage), columns);
    int32_t left = BADGES_RIGHT + 1 - (int32_t)all;
    uint32_t count = badge_columns(stage, (uint32_t)shown, columns);
    for (uint32_t i = 0; i < count; i++) {
        const TileColors colors = char_colors(columns[i].palette);
        draw_char(fb, columns[i].top, left + (int32_t)i, BOTTOM_ROW, &colors);
        draw_char(fb, columns[i].bottom, left + (int32_t)i, BOTTOM_ROW + 1, &colors);
    }
}

void draw_reserve(Framebuffer *fb, int32_t reserve)
{
    const TileColors colors = char_colors(BOTTOM_PALETTE);
    int32_t shown = reserve < RESERVE_SHOWN ? reserve : RESERVE_SHOWN;
    for (int32_t i = 0; i < shown; i++)
        for (int32_t k = 0; k < 4; k++)
            draw_char(fb, RESERVE_CHARS[k], i * 2 + (k & 1), BOTTOM_ROW + (k >> 1), &colors);
}
