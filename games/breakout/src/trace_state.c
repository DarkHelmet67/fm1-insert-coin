/* SPDX-License-Identifier: GPL-3.0-only */
#include "trace_state.h"

/** `|name=` before a field. */
static void add_name(Text *out, const char *name)
{
    text_add_char(out, '|');
    text_add(out, name);
}

/** A number and a ':' after it. */
static void add_part(Text *out, int32_t value)
{
    text_add_int(out, value);
    text_add_char(out, ':');
}

/** The phase: r, s:frames:pressed, i:x:y:dx:dy:hits:fast:outer:can, or o. */
static void add_play(Text *out, const Play *play)
{
    const Ball *b = &play->ball;
    add_name(out, "pl=");
    switch (play->phase) {
    case PHASE_READY:
        text_add_char(out, 'r');
        break;
    case PHASE_SERVING:
        text_add(out, "s:");
        add_part(out, (int32_t)play->frames_left);
        text_add_int(out, (int32_t)play->pressed_at);
        break;
    case PHASE_IN_PLAY:
        text_add(out, "i:");
        add_part(out, b->x_fifths);
        add_part(out, b->y_fifths);
        add_part(out, b->dir_x);
        add_part(out, b->dir_y);
        add_part(out, (int32_t)b->hits);
        add_part(out, b->fast);
        add_part(out, b->outer);
        text_add_int(out, b->can_hit_brick);
        break;
    default:
        text_add_char(out, 'o');
        break;
    }
}

/** The wall, four bricks a hex digit, the first brick as the highest bit. */
static void add_wall(Text *out, const Wall *wall)
{
    add_name(out, "w=");
    for (uint32_t i = 0; i < BRICK_COUNT; i += 4u) {
        const uint32_t digit = (uint32_t)wall->standing[i] << 3 |
                               (uint32_t)wall->standing[i + 1u] << 2 |
                               (uint32_t)wall->standing[i + 2u] << 1 | wall->standing[i + 3u];
        text_add_hex(out, digit, 1);
    }
}

/** `|name=value`. */
static void add_field(Text *out, const char *name, int32_t value)
{
    add_name(out, name);
    text_add_int(out, value);
}

void trace_state(const GameState *state, Text *out)
{
    const Round *round = &state->round;
    text_clear(out);
    text_add_char(out, trace_screen_letter(state));
    add_play(out, &round->play);
    add_wall(out, &round->wall);
    add_field(out, "s=", round->score);
    add_field(out, "b=", (int32_t)round->ball);
    add_field(out, "sh=", round->shrunk);
    add_field(out, "rf=", round->refilled);
    add_name(out, "p=");
    add_part(out, state->paddle.x);
    text_add_int(out, state->paddle.width);
    add_field(out, "clk=", (int32_t)state->clock);
    add_field(out, "hi=", state->hi_score);
    add_field(out, "rec=", state->record_to_beat);
    add_name(out, "t=");
    add_part(out, state->ticks.owed);
    add_part(out, (int32_t)state->ticks.played);
    text_add_int(out, (int32_t)state->ticks.wait);
}

char trace_screen_letter(const GameState *state)
{
    return state->attract ? 'A' : 'P';
}
