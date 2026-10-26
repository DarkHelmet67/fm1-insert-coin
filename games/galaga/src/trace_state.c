/* SPDX-License-Identifier: GPL-3.0-only */
#include "trace_state.h"

#include "trace_parts.h"

/** `name` and a number in decimal. */
static void int_field(Text *out, const char *name, int32_t value)
{
    trace_field(out, name);
    text_add_int(out, value);
}

/** The star field: `phase:speed`. */
static void stars_field(Text *out, const StarField *stars)
{
    trace_field(out, "sf=");
    text_add_int(out, stars->phase);
    text_add_char(out, ':');
    text_add_int(out, stars->speed);
}

/** The drift of the bombs and the paths of the rockets. */
static void shots_fields(Text *out, const PlayingState *p)
{
    trace_field(out, "bd=");
    for (int32_t i = 0; i < BOMB_COUNT; i++) {
        if (i > 0)
            text_add_char(out, ',');
        text_add_int(out, p->bomb_drift[i].rate);
        text_add_char(out, ':');
        text_add_int(out, p->bomb_drift[i].accumulator);
    }
    trace_field(out, "rp=");
    for (int32_t i = 0; i < ROCKET_COUNT; i++) {
        if (i > 0)
            text_add_char(out, ',');
        text_add_int(out, p->rocket_paths.path[i].dx);
        text_add_char(out, ':');
        text_add_int(out, p->rocket_paths.path[i].dy);
    }
}

/** The first half of a game: the program, the objects and the grid. */
static void playing_world(Text *out, const PlayingState *p)
{
    text_add(out, "f=");
    text_add_int(out, p->frame);
    trace_field(out, "t=");
    trace_timers(out, &p->timers);
    trace_field(out, "ph=");
    trace_phase(out, &p->phase);
    int_field(out, "tk=", p->tasks);
    trace_field(out, "o=");
    trace_objects(out, &p->objects);
    trace_field(out, "sl=");
    trace_slots(out, &p->slots);
    shots_fields(out, p);
    int_field(out, "stt=", p->step_toggle);
    int_field(out, "du=", p->dual);
    trace_field(out, "fm=");
    trace_formation(out, &p->formation);
    stars_field(out, &p->stars);
    trace_field(out, "sc=");
    trace_star_control(out, &p->star_control);
}

/** The second half of a game: the stage, the attacks, the capture and the counters. */
static void playing_stage(Text *out, const PlayingState *p)
{
    int_field(out, "stg=", p->stage);
    int_field(out, "rk=", p->rank);
    trace_field(out, "pa=");
    trace_params(out, &p->params);
    trace_field(out, "lk=");
    trace_looks(out, &p->looks);
    trace_field(out, "la=");
    trace_launcher(out, &p->launcher);
    int_field(out, "ebb=", p->entry_bomb_bits);
    int_field(out, "bi=", p->bomb_interval);
    trace_field(out, "at=");
    trace_attacks(out, &p->attacks);
    int_field(out, "mb=", p->max_bombers);
    int_field(out, "fh=", p->flying_hits);
    int_field(out, "gl=", p->group_left);
    trace_field(out, "cp=");
    trace_capture(out, &p->capture);
    int_field(out, "tl=", p->triple_left);
    int_field(out, "tb=", p->triple_bonus.hundreds);
    text_add_char(out, ':');
    trace_hex(out, p->triple_bonus.code);
}

/** The flags and counters at the end of a game's string. */
static void playing_counters(Text *out, const PlayingState *p)
{
    int_field(out, "en=", p->enemies_enabled);
    int_field(out, "we=", p->waves_enabled);
    int_field(out, "ss=", p->sway_stops);
    int_field(out, "al=", p->alive);
    int_field(out, "cn=", p->counting);
    int_field(out, "rs=", p->restart);
    int_field(out, "rv=", p->reserve);
    int_field(out, "bg=", p->badges);
    trace_field(out, "ms=");
    trace_messages(out, &p->messages);
    int_field(out, "sco=", p->score);
    int_field(out, "nb=", p->next_bonus);
    int_field(out, "sh=", p->shots);
    int_field(out, "hi=", p->hits);
    trace_field(out, "snd=");
    trace_hex(out, (int32_t)p->sounds);
}

/** The attract screen. */
static void attract_string(Text *out, const Attract *a)
{
    static const char *const STEPS[] = {"info", "copyright"};
    text_add(out, "f=");
    text_add_int(out, a->frame);
    stars_field(out, &a->stars);
    trace_field(out, "t=");
    trace_timers(out, &a->timers);
    trace_field(out, "st=");
    text_add(out, STEPS[a->step & 1]);
    int_field(out, "n=", a->shown);
    trace_field(out, "ms=");
    trace_messages(out, &a->messages);
    trace_field(out, "o=");
    trace_objects(out, &a->objects);
}

void trace_state(const GameState *state, Text *out)
{
    text_clear(out);
    text_add(out, state->screen == SCREEN_ATTRACT ? "A|hi=" : "P|hi=");
    text_add_int(out, state->hi_score);
    text_add_char(out, '|');
    if (state->screen == SCREEN_ATTRACT) {
        attract_string(out, &state->on.attract);
        return;
    }
    playing_world(out, &state->on.playing);
    playing_stage(out, &state->on.playing);
    playing_counters(out, &state->on.playing);
}

char trace_screen_letter(const GameState *state)
{
    return state->screen == SCREEN_ATTRACT ? 'A' : 'P';
}
