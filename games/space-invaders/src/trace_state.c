/* SPDX-License-Identifier: GPL-3.0-only */
#include "trace_state.h"

/** The letter of each invader and bomb kind. */
static const char ALIEN_LETTERS[] = "sco";
static const char BOMB_LETTERS[] = "rpq";

/** The name of each effect kind. */
static const char *const EFFECT_NAMES[] = {"alien", "shot", "bomb", "ufo"};

/** `name=value` followed by the field separator. */
static void add_field(Text *out, const char *name, int32_t value)
{
    text_add(out, name);
    text_add_int(out, value);
    text_add_char(out, ';');
}

/** Cursor, direction, drop and beat of the formation, then every invader. */
static void add_fleet(Text *out, const Fleet *fleet)
{
    text_add(out, "fl=");
    text_add_int(out, fleet->cursor);
    text_add_char(out, ':');
    text_add_int(out, fleet->direction);
    text_add_char(out, ':');
    text_add_int(out, fleet->dropping);
    text_add_char(out, ':');
    text_add_int(out, (int32_t)fleet->beat);
    text_add_char(out, ':');
    text_add_int(out, (int32_t)fleet->beat_cooldown);
    text_add_char(out, ':');
    for (uint32_t i = 0; i < fleet->count; i++) {
        const Alien *a = &fleet->aliens[i];
        if (i)
            text_add_char(out, ',');
        text_add_char(out, ALIEN_LETTERS[a->kind]);
        text_add_int(out, a->row);
        text_add_char(out, '.');
        text_add_int(out, a->column);
        text_add_char(out, '@');
        text_add_int(out, a->x);
        text_add_char(out, ':');
        text_add_int(out, a->y);
        text_add_char(out, ':');
        text_add_int(out, a->frame);
    }
    text_add_char(out, ';');
}

/** The firing tables' positions, then every bomb. */
static void add_bombs(Text *out, const Bombs *bombs)
{
    text_add(out, "b=");
    text_add_int(out, (int32_t)bombs->turn);
    text_add_char(out, ':');
    text_add_int(out, bombs->plunger_index);
    text_add_char(out, ':');
    text_add_int(out, bombs->squiggly_index);
    text_add_char(out, ':');
    for (uint32_t i = 0; i < bombs->count; i++) {
        const Bomb *b = &bombs->active[i];
        if (i)
            text_add_char(out, ',');
        text_add_char(out, BOMB_LETTERS[b->kind]);
        text_add_char(out, '@');
        text_add_int(out, b->x);
        text_add_char(out, ':');
        text_add_int(out, b->y);
        text_add_char(out, ':');
        text_add_int(out, b->steps);
    }
    text_add_char(out, ';');
}

/** Every shield: position, then its 22 x 16 bitmap as 6 hex digits per row. */
static void add_shields(Text *out, const Shields *shields)
{
    text_add(out, "sd=");
    for (uint32_t i = 0; i < SHIELD_COUNT; i++) {
        const Shield *s = &shields->list[i];
        if (i)
            text_add_char(out, ',');
        text_add_int(out, s->x);
        text_add_char(out, ':');
        text_add_int(out, s->y);
        text_add_char(out, ':');
        /* bit 31 is the leftmost pixel: the top 24 bits are the row padded to 6 hex digits */
        for (uint32_t y = 0; y < s->bitmap.height; y++)
            text_add_hex(out, s->bitmap.rows[y] >> 8, 6);
    }
    text_add_char(out, ';');
}

/** The mystery ship's countdown, then the ship or '-'. */
static void add_ufo(Text *out, const Ufo *ufo)
{
    text_add(out, "u=");
    text_add_int(out, (int32_t)ufo->countdown);
    text_add_char(out, ':');
    if (!ufo->flying) {
        text_add_char(out, '-');
    } else {
        text_add_int(out, ufo->x);
        text_add_char(out, ':');
        text_add_int(out, ufo->direction);
        text_add_char(out, ':');
        text_add_int(out, (int32_t)ufo->age);
    }
    text_add_char(out, ';');
}

/** Every effect, with the points of the ship's. */
static void add_effects(Text *out, const Effects *effects)
{
    text_add(out, "e=");
    for (uint32_t i = 0; i < effects->count; i++) {
        const Effect *e = &effects->list[i];
        if (i)
            text_add_char(out, ',');
        text_add(out, EFFECT_NAMES[e->kind]);
        text_add_char(out, '@');
        text_add_int(out, e->x);
        text_add_char(out, ':');
        text_add_int(out, e->y);
        text_add_char(out, ':');
        text_add_int(out, e->frames_left);
        if (e->points != NO_POINTS) {
            text_add_char(out, ':');
            text_add_int(out, e->points);
        }
    }
}

/** The fields of a game, separated by ';'. */
static void add_playing(Text *out, const PlayingState *p)
{
    add_field(out, "r=", (int32_t)p->round);
    add_field(out, "s=", p->score);
    add_field(out, "l=", p->lives);
    add_field(out, "xl=", p->extra_life_awarded);
    add_field(out, "sf=", (int32_t)p->shots_fired);
    add_field(out, "c=", p->cannon_x);
    if (p->cannon_explosion)
        add_field(out, "ce=", (int32_t)p->cannon_explosion);
    else
        text_add(out, "ce=-;");
    if (p->shot.active) {
        text_add(out, "sh=");
        text_add_int(out, p->shot.x);
        text_add_char(out, ':');
        text_add_int(out, p->shot.y);
        text_add_char(out, ';');
    } else {
        text_add(out, "sh=-;");
    }
    add_fleet(out, &p->fleet);
    add_bombs(out, &p->bombs);
    add_shields(out, &p->shields);
    add_ufo(out, &p->ufo);
    add_effects(out, &p->effects);
}

void trace_state(const GameState *state, Text *out)
{
    text_clear(out);
    switch (state->screen) {
    case SCREEN_PLAYING:
        text_add(out, "P|hi=");
        text_add_int(out, state->hi_score);
        text_add(out, "|rec=");
        text_add_int(out, state->record_to_beat);
        text_add_char(out, '|');
        add_playing(out, &state->playing);
        break;
    case SCREEN_GAME_OVER:
        text_add(out, "O|hi=");
        text_add_int(out, state->hi_score);
        text_add(out, "|left=");
        text_add_int(out, (int32_t)state->frames_left);
        text_add_char(out, '|');
        add_playing(out, &state->playing);
        break;
    default:
        text_add(out, "A|hi=");
        text_add_int(out, state->hi_score);
        break;
    }
}

char trace_screen_letter(const GameState *state)
{
    switch (state->screen) {
    case SCREEN_PLAYING:
        return 'P';
    case SCREEN_GAME_OVER:
        return 'G';
    default:
        return 'A';
    }
}
