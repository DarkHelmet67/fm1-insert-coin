/* SPDX-License-Identifier: GPL-3.0-only */
#include "hud.h"

#include "config.h"
#include "screen/seven_segment.h"

/** The digits' size and strokes (tuning.config.ts digits). */
static const SegmentStyle DIGIT_STYLE = {DIGIT_WIDTH, DIGIT_HEIGHT, DIGIT_STROKE_X, DIGIT_STROKE_Y};

void hud_format_score(int32_t score, char out[HUD_SCORE_CHARS])
{
    const int32_t shown = score % 1000;
    out[0] = (char)('0' + shown / 100);
    out[1] = (char)('0' + shown / 10 % 10);
    out[2] = (char)('0' + shown % 10);
    out[3] = '\0';
}

int hud_blink_on(uint32_t clock)
{
    return clock * 2u * SCORE_BLINK_HZ / FRAMES_PER_SECOND % 2u == 0u;
}

/** One digit, 1-9, as a string. */
static void one_digit(uint32_t value, char out[2])
{
    out[0] = (char)('0' + value % 10u);
    out[1] = '\0';
}

void hud_render(Framebuffer *fb, const GameState *state, uint8_t color)
{
    char text[HUD_SCORE_CHARS];
    one_digit(1u, text);
    fb_draw_segment_number(fb, text, DIGITS_LEFT_X, DIGITS_UPPER_Y, DIGIT_PITCH, &DIGIT_STYLE,
                           color);
    /* as on the cabinet, the score of the player up blinks while the game is on */
    if (state->attract || hud_blink_on(state->clock)) {
        hud_format_score(state->round.score, text);
        fb_draw_segment_number(fb, text, DIGITS_LEFT_X, DIGITS_LOWER_Y, DIGIT_PITCH, &DIGIT_STYLE,
                               color);
    }
    one_digit(state->round.ball, text);
    fb_draw_segment_number(fb, text, DIGITS_RIGHT_X, DIGITS_UPPER_Y, DIGIT_PITCH, &DIGIT_STYLE,
                           color);
    /* the second player's place shows the record: the remake has one player and keeps one */
    hud_format_score(state->hi_score, text);
    fb_draw_segment_number(fb, text, DIGITS_RIGHT_X, DIGITS_LOWER_Y, DIGIT_PITCH, &DIGIT_STYLE,
                           color);
}
