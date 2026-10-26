/* SPDX-License-Identifier: GPL-3.0-only */
#include "hits.h"

/** The hit notification of an alien that explodes with no score sprite. */
#define HIT_PLAIN 0x81

/** The palettes that matter to a hit. */
#define COLOR_GREEN_BOSS 0
#define COLOR_BLUE_BOSS 1
#define COLOR_CAPTURED 7
#define COLOR_BOMB 0x0b

/** The bonus counter: hundreds of points. */
#define BONUS_COUNTER 15

/** The hit sound of each palette (`b_9AA0[1 + n]`). */
static const uint32_t HIT_SOUNDS[4] = {SOUND_BOSS_HIT, SOUND_BUTTERFLY_HIT, SOUND_BEE_HIT,
                                       SOUND_BOSS_DAMAGED};

HitReport no_hit_report(void)
{
    HitReport report;
    for (int32_t i = 0; i < HIT_COUNTERS; i++)
        report.hit_counts[i] = 0;
    report.quiet_timer = -1;
    report.fighters_count = 0;
    return report;
}

/** Sets the hit notification of object `id`. */
static void notify(PlayingState *state, int32_t id, int32_t hit)
{
    object_ref(&state->objects, id)->hit = (uint8_t)hit;
}

/** `amount` more hits of palette `color`. */
static void count(HitReport *report, int32_t color, int32_t amount)
{
    report->hit_counts[color & 0x0f] += amount;
}

/** `l_07DF`: the common end of a hit on an alien: capture state, sound and counters. */
static void score_hit(PlayingState *state, HitReport *report, int32_t id, int32_t color, int flying)
{
    /* The capturing Boss is shot: a new capture may start. */
    if (state->capture.object == id) {
        state->capture.busy = 0;
        state->capture.object = 1;
    }
    /* A captured fighter is shot: it is lost for good, and a new capture may start. */
    if (color == COLOR_CAPTURED) {
        state->capture.busy = 0;
        play(state, SOUND_CAPTURED_FIGHTER_HIT);
    } else {
        play(state, HIT_SOUNDS[(color - 1) & 3]);
    }
    count(report, color, flying ? 2 : 1);
}

/** `l_07DB`: the alien will explode, with no score sprite. */
static void plain_hit(PlayingState *state, HitReport *report, int32_t id, int32_t color, int flying)
{
    notify(state, id, HIT_PLAIN);
    score_hit(state, report, id, color, flying);
}

/** A hit that shows score sprite `code` and adds a bonus of `hundreds` hundred points. */
static void bonus_hit(PlayingState *state, HitReport *report, int32_t id, int32_t color,
                      int32_t hundreds, int32_t code)
{
    notify(state, id, code);
    count(report, BONUS_COUNTER, hundreds);
    score_hit(state, report, id, color, 1);
}

/**
 * `l_0849`: a diving Boss carrying a captured fighter is shot. The fighter leaves its flight
 * slot, turns white and starts spinning down to join the player's (task `f_2000`).
 */
static void rescue(PlayingState *state, int32_t fighter)
{
    GameObject *captured = object_ref(&state->objects, fighter);
    if (captured->disposition != DISPOSITION_FLYING)
        return;
    captured->color = 9;
    captured->disposition = 0;
    if (captured->counter < SLOT_COUNT && state->slots.list[captured->counter].used &&
        state->slots.list[captured->counter].object == fighter)
        state->slots.list[captured->counter] = free_slot();
    state->capture.object = (uint8_t)fighter;
    state->capture.step = 0;
    state->capture.pulling = 1;
    set_capture_tasks(&state->capture, CAPTURE_RESCUE, 0);
}

/** `l_0849`: the special cases of a flying alien hit. */
static void flying_special(PlayingState *state, HitReport *report, int32_t id, int32_t color)
{
    if (color == COLOR_CAPTURED) {
        notify(state, id, 0xb8);
        score_hit(state, report, id, color, 1);
    } else if (id == state->attacks.bonus_bee || (id & 0x38) == 0x38) {
        state->triple_left = (state->triple_left - 1) & 0xff;
        if (state->triple_left == 0)
            bonus_hit(state, report, id, color, state->triple_bonus.hundreds,
                      state->triple_bonus.code);
        else
            plain_hit(state, report, id, color, 1);
    } else if (color != COLOR_BLUE_BOSS) {
        plain_hit(state, report, id, color, 1);
    } else {
        BossScore score = state->attacks.boss_scores[(id & 7) >> 1];
        rescue(state, id & 7);
        report->quiet_timer = 6;
        bonus_hit(state, report, id, color, score.hundreds, score.code);
    }
}

/** `l_081E`: a flying alien is hit: its flight slot is freed and the group count goes down. */
static void flying_hit(PlayingState *state, HitReport *report, int32_t id, int32_t color,
                       const HitInputs *inputs)
{
    const GameObject *target = object_at(&state->objects, id);
    int32_t group_left = state->group_left;
    if (target->counter < SLOT_COUNT && state->slots.list[target->counter].used &&
        state->slots.list[target->counter].object == id)
        state->slots.list[target->counter] = free_slot();
    state->flying_hits++;
    /* Only challenging stages count groups: elsewhere the counter stays at 0. */
    state->group_left = group_left == 0 ? 0 : group_left - 1;
    if (group_left != 1) {
        flying_special(state, report, id, color);
        return;
    }
    notify(state, id, inputs->challenge_bonus.score_code);
    count(report, BONUS_COUNTER, inputs->challenge_bonus.tens);
    score_hit(state, report, id, color, 1);
}

/**
 * `hitd_dspchr`: what a hit does, by the target's palette: a green Boss turns blue, a bomb
 * disappears, an alien starts to explode.
 */
static void dispatch(PlayingState *state, HitReport *report, int32_t id, int flying,
                     const HitInputs *inputs)
{
    GameObject *target = object_ref(&state->objects, id);
    int32_t color = target->color;
    if (color == COLOR_GREEN_BOSS) {
        target->color = COLOR_BLUE_BOSS;
        play(state, SOUND_BOSS_DAMAGED);
    } else if (color == COLOR_BOMB) {
        target->x = 0;
        target->disposition = DISPOSITION_INACTIVE;
    } else if (flying) {
        flying_hit(state, report, id, color, inputs);
    } else {
        plain_hit(state, report, id, color, 0);
    }
}

/** Whether an object can be hit: in play, not exploding, not already hit. */
static int hittable(const GameObject *object)
{
    return ((object->hit | object->disposition) & 0x80) == 0 && (object->disposition & 0xfe) != 4;
}

/** The signed distance of two bytes. */
static int32_t delta(int32_t a, int32_t b)
{
    return ((a - b + 0x80) & 0xff) - 0x80;
}

/** `hitd_det_rckt`: a rocket within reach of an alien; the double fighter's rocket is wider. */
static int rocket_reaches(const GameObject *rocket, const GameObject *target, int dual)
{
    int32_t dy = delta(target->y >> 1, rocket->y >> 1);
    if (dy < -3 || dy > 2)
        return 0;
    int32_t dx = delta(target->x, rocket->x);
    return dual ? (dx >= -6 && dx <= 4) || (dx >= 9 && dx <= 19) : dx >= -5 && dx <= 5;
}

/**
 * One rocket against the aliens. The rocket disappears on a hit, but the check goes on with the
 * rocket where it was: one rocket can hit two aliens side by side.
 */
static void rocket_hits(PlayingState *state, HitReport *report, int32_t rocket_id,
                        const HitInputs *inputs)
{
    const GameObject rocket = *object_at(&state->objects, rocket_id);
    if (rocket.x == 0)
        return;
    int32_t first = inputs->rescue_running ? 4 : 0;
    for (int32_t i = first; i < ENEMY_OBJECTS; i++) {
        int32_t id = i * 2;
        const GameObject *target = object_at(&state->objects, id);
        if (!hittable(target) || !rocket_reaches(&rocket, target, inputs->dual))
            continue;
        int flying = ((target->disposition - 1) & 0xfe) != 0;
        GameObject *spent = object_ref(&state->objects, rocket_id);
        state->hits++;
        spent->x = 0;
        spent->wide = 0;
        dispatch(state, report, id, flying, inputs);
    }
}

/** `hitd_det_fghtr`: an object touching the fighter. */
static int touches(const GameObject *fighter, const GameObject *target)
{
    int32_t dx = delta(target->x, fighter->x);
    int32_t dy = delta(target->y >> 1, fighter->y >> 1);
    return hittable(target) && target->x != 0 && dx >= -6 && dx <= 6 && dy >= -3 && dy <= 3;
}

/** The first of `count` objects from `first` touching the fighter, or -1. */
static int32_t first_touching(const Objects *objects, const GameObject *fighter, int32_t first,
                              int32_t count)
{
    for (int32_t i = 0; i < count; i++)
        if (touches(fighter, object_at(objects, first + i * 2)))
            return first + i * 2;
    return -1;
}

/**
 * `f_05EE`: one fighter against aliens (only special enemies while waves enter) and bombs. An
 * alien that touches the fighter is destroyed too.
 */
static void fighter_hits(PlayingState *state, HitReport *report, const HitInputs *inputs,
                         int32_t fighter_id)
{
    const GameObject fighter = *object_at(&state->objects, fighter_id);
    if (fighter.x == 0 || fighter.disposition == DISPOSITION_FIGHTER_EXPLODING)
        return;
    int32_t alien = inputs->waves_entering
                        ? first_touching(&state->objects, &fighter, 0x38, 4)
                        : first_touching(&state->objects, &fighter, 0, ENEMY_OBJECTS);
    int32_t touching =
        alien >= 0 ? alien : first_touching(&state->objects, &fighter, FIRST_BOMB, BOMB_COUNT);
    if (touching < 0)
        return;
    report->fighters_hit[report->fighters_count++] = (uint8_t)fighter_id;
    dispatch(state, report, touching, 1, inputs);
}

void detect_hits(PlayingState *state, const HitInputs *inputs, int fighter_checked,
                 HitReport *report)
{
    rocket_hits(state, report, FIRST_ROCKET, inputs);
    rocket_hits(state, report, FIRST_ROCKET + 2, inputs);
    if (!fighter_checked)
        return;
    if (inputs->dual)
        fighter_hits(state, report, inputs, FIGHTER_2);
    fighter_hits(state, report, inputs, FIGHTER);
}

void start_explosions(Objects *objects)
{
    for (int32_t i = 0; i < ENEMY_OBJECTS; i++) {
        GameObject *object = &objects->list[i];
        if ((object->hit & 0x80) == 0)
            continue;
        object->hit &= 0x7f;
        object->disposition = DISPOSITION_EXPLODING;
        object->counter = 0x40;
        object->color = 0x0a;
    }
}

void explode_fighter(Objects *objects, int32_t id, int32_t x, int32_t y)
{
    GameObject *object = object_ref(objects, id);
    object->x = (uint8_t)(x - 8);
    object->y = (uint16_t)((y - 8) & 0x1ff);
    object->code = 0x20;
    object->color = 0x0b;
    object->disposition = DISPOSITION_FIGHTER_EXPLODING;
    object->counter = 0x0f;
    object->big = 1;
    object->flip_x = 0;
    object->flip_y = 0;
}
