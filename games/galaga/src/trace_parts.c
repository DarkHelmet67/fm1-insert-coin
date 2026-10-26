/* SPDX-License-Identifier: GPL-3.0-only */
#include "trace_parts.h"

void trace_hex(Text *out, int32_t value)
{
    uint32_t magnitude = value < 0 ? (uint32_t)-value : (uint32_t)value;
    uint32_t digits = 1;
    while (digits < 8 && (magnitude >> (digits * 4)) != 0)
        digits++;
    if (value < 0)
        text_add_char(out, '-');
    text_add_hex(out, magnitude, digits);
}

void trace_field(Text *out, const char *name)
{
    text_add_char(out, '|');
    text_add(out, name);
}

/** `value` and a separator after it. */
static void hex_then(Text *out, int32_t value, char separator)
{
    trace_hex(out, value);
    text_add_char(out, separator);
}

/** `value` in decimal and a separator after it. */
static void int_then(Text *out, int32_t value, char separator)
{
    text_add_int(out, value);
    text_add_char(out, separator);
}

/** Up to four flags as one number: the first is bit 0. */
static int32_t bits4(int a, int b, int c, int d)
{
    return (a ? 1 : 0) | (b ? 2 : 0) | (c ? 4 : 0) | (d ? 8 : 0);
}

void trace_objects(Text *out, const Objects *objects)
{
    for (int32_t i = 0; i < OBJECT_COUNT; i++) {
        const GameObject *o = &objects->list[i];
        if (i > 0)
            text_add_char(out, ',');
        hex_then(out, o->disposition, '.');
        hex_then(out, o->counter, '.');
        hex_then(out, o->code, '.');
        hex_then(out, o->color, '.');
        hex_then(out, o->x, '.');
        hex_then(out, o->y, '.');
        hex_then(out, bits4(o->flip_x, o->flip_y, o->big, o->wide), '.');
        trace_hex(out, o->hit);
    }
}

/** One flight slot. */
static void trace_slot(Text *out, const FlightSlot *s)
{
    if (!s->used) {
        text_add_char(out, '-');
        return;
    }
    hex_then(out, s->y, '.');
    hex_then(out, s->x, '.');
    hex_then(out, s->angle, '.');
    hex_then(out, s->target_y, '.');
    hex_then(out, s->target_x, '.');
    hex_then(out, s->pointer, '.');
    hex_then(out, s->speed_odd, '.');
    hex_then(out, s->speed_even, '.');
    hex_then(out, s->turn, '.');
    hex_then(out, s->timer, '.');
    hex_then(out, s->bomb_timer, '.');
    hex_then(out, s->bomb_bits, '.');
    hex_then(out, s->object, '.');
    hex_then(out, s->offset_x, '.');
    hex_then(out, s->offset_y, '.');
    trace_hex(out, bits4(s->dive_check, s->homing, s->mirrored, 0));
}

void trace_slots(Text *out, const Slots *slots)
{
    for (int32_t i = 0; i < SLOT_COUNT; i++) {
        if (i > 0)
            text_add_char(out, ',');
        trace_slot(out, &slots->list[i]);
    }
}

void trace_timers(Text *out, const GameTimers *timers)
{
    for (int32_t i = 0; i < 4; i++) {
        if (i > 0)
            text_add_char(out, ',');
        text_add_int(out, timers->t[i]);
    }
}

void trace_messages(Text *out, const Messages *messages)
{
    for (uint32_t i = 0; i < messages->count; i++) {
        const Message *m = &messages->list[i];
        if (i > 0)
            text_add_char(out, ';');
        text_add(out, m->text);
        text_add_char(out, '@');
        int_then(out, m->column, ':');
        int_then(out, m->row, ':');
        text_add_int(out, m->palette);
    }
}

void trace_phase(Text *out, const Phase *phase)
{
    static const char *const STEPS[] = {"intro",   "splash",  "respawn", "respawnDelay",
                                        "running", "restart", "ending",  "over"};
    static const char *const THENS[] = {"playerUp", "respawn", "ready"};
    static const char *const SCRIPTS[] = {"challenge", "perfect", "gameOver"};
    text_add(out, STEPS[phase->step & 7]);
    if (phase->step == STEP_SPLASH) {
        text_add_char(out, ':');
        text_add(out, THENS[phase->then % 3]);
    } else if (phase->step == STEP_ENDING) {
        text_add_char(out, ':');
        text_add(out, SCRIPTS[phase->script % 3]);
        text_add_char(out, ':');
        int_then(out, phase->line, ':');
        text_add_int(out, phase->waiting);
    }
}

/** A list of bytes in decimal, separated by `,`. */
static void byte_list(Text *out, const uint8_t *values, int32_t count)
{
    for (int32_t i = 0; i < count; i++) {
        if (i > 0)
            text_add_char(out, ',');
        text_add_int(out, values[i]);
    }
}

void trace_formation(Text *out, const Formation *formation)
{
    byte_list(out, formation->offsets, CELLS);
    text_add_char(out, '/');
    for (int32_t i = 0; i < CELLS; i++) {
        if (i > 0)
            text_add_char(out, ',');
        text_add_int(out, formation->sprite_coords[i]);
    }
    text_add_char(out, '/');
    int_then(out, formation->direction, '/');
    byte_list(out, formation->bitmap, CELLS);
}

void trace_params(Text *out, const StageParameters *p)
{
    int_then(out, p->bomb_enable, ',');
    int_then(out, p->boss_timer, ',');
    int_then(out, p->red_timer, ',');
    int_then(out, p->yellow_timer, ',');
    int_then(out, p->max_bombers, ',');
    int_then(out, p->max_bombers_later, ',');
    int_then(out, p->beam_speed, ',');
    int_then(out, p->continuous_bombing, ',');
    int_then(out, p->harder_entries != 0, ',');
    int_then(out, p->harder_attacks != 0, ',');
    text_add_int(out, p->clone_attack);
}

void trace_looks(Text *out, const Looks *looks)
{
    int first = 1;
    for (int32_t i = 0; i < OBJECT_COUNT; i++) {
        if (!looks->known[i])
            continue;
        if (!first)
            text_add_char(out, ',');
        first = 0;
        hex_then(out, i * 2, ':');
        hex_then(out, looks->look[i].code, ':');
        int_then(out, looks->look[i].color, ':');
        text_add_int(out, looks->look[i].bombs != 0);
    }
}

void trace_launcher(Text *out, const Launcher *launcher)
{
    for (int32_t i = 0; i < launcher->count; i++) {
        const WaveToken *token = &launcher->tokens[i];
        if (i > 0)
            text_add_char(out, ',');
        if (token->kind == TOKEN_START) {
            text_add_char(out, 's');
        } else if (token->kind == TOKEN_END) {
            text_add_char(out, 'e');
        } else {
            hex_then(out, token->control, '.');
            trace_hex(out, token->object);
        }
    }
    text_add_char(out, '/');
    int_then(out, launcher->next, '/');
    int_then(out, launcher->wave_count, '/');
    text_add_int(out, launcher->running);
}

void trace_attacks(Text *out, const Attacks *a)
{
    byte_list(out, a->countdown, 3);
    text_add_char(out, '/');
    for (int32_t i = 0; i < a->queue_length; i++) {
        if (i > 0)
            text_add_char(out, ',');
        hex_then(out, a->queue[i].object, ':');
        int_then(out, a->queue[i].mirrored, ':');
        trace_hex(out, a->queue[i].program);
    }
    text_add_char(out, '/');
    int_then(out, a->boss_toggle, '/');
    for (int32_t i = 0; i < 4; i++) {
        if (i > 0)
            text_add_char(out, ',');
        int_then(out, a->boss_scores[i].hundreds, ':');
        trace_hex(out, a->boss_scores[i].code);
    }
    text_add_char(out, '/');
    hex_then(out, a->bonus_bee, '/');
    int_then(out, a->bonus_bee_timer, '/');
    hex_then(out, a->bonus_bee_look, '/');
    text_add_int(out, a->bonus_bee_color);
}

void trace_capture(Text *out, const Capture *c)
{
    int_then(out, c->busy, ',');
    hex_then(out, c->object, ',');
    int_then(out, c->slot, ',');
    int_then(out, c->beam_x, ',');
    hex_then(out, c->step, ',');
    int_then(out, c->countdown, ',');
    int_then(out, c->pulling, ',');
    int_then(out, c->show_text, ',');
    int_then(out, c->beam_speed, ',');
    int_then(out, c->connected, ',');
    int_then(out, c->beam_rows, ',');
    text_add_int(out, c->tasks);
}

void trace_star_control(Text *out, const StarControl *c)
{
    int_then(out, c->ship_on_screen, ',');
    int_then(out, c->backwards, ',');
    int_then(out, c->target, ',');
    int_then(out, c->current, ',');
    text_add_int(out, c->remainder);
}
