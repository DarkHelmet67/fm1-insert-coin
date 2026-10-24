/* SPDX-License-Identifier: GPL-3.0-only */
#include "bombs.h"

/**
 * Columns (1-11) that drop plunger and squiggly bombs, in order. Plungers read the first 16
 * entries; squiggly bombs read the same sequence starting at entry 6 (11, 1, 6, 3, ...), as
 * documented for the original.
 */
static const uint8_t COLUMN_FIRE_TABLE[21] = {1, 7,  1, 1, 1, 4, 11, 1, 6, 3, 1,
                                              1, 11, 9, 2, 8, 2, 11, 4, 7, 10};
#define PLUNGER_START 0u
#define PLUNGER_LENGTH 16u
#define SQUIGGLY_START 6u
#define SQUIGGLY_LENGTH 15u

Bombs bombs_none(void)
{
    Bombs bombs;
    bombs.count = 0;
    bombs.plunger_index = 0;
    bombs.squiggly_index = 0;
    bombs.turn = 0;
    return bombs;
}

int32_t bomb_speed(uint32_t aliens_left)
{
    return aliens_left <= 8u ? 5 : 4;
}

uint32_t reload_steps(int32_t score)
{
    if (score < 200)
        return 48;
    if (score < 1000)
        return 16;
    if (score < 2000)
        return 11;
    if (score < 3000)
        return 8;
    return 7;
}

int32_t lowest_in_column(const Fleet *fleet, uint32_t column)
{
    int32_t lowest = -1;
    for (uint32_t i = 0; i < fleet->count; i++) {
        const Alien *alien = &fleet->aliens[i];
        if (alien->column == column && (lowest < 0 || alien->y > fleet->aliens[lowest].y))
            lowest = (int32_t)i;
    }
    return lowest;
}

uint32_t column_above(const Fleet *fleet, int32_t x)
{
    for (uint32_t i = 0; i < fleet->count; i++) {
        const Alien *alien = &fleet->aliens[i];
        if (x >= alien->x && x < alien->x + alien_width(alien))
            return alien->column;
    }
    return 0;
}

/** A new bomb leaving the bottom centre of `alien`. */
static Bomb drop_from(BombKind kind, const Alien *alien)
{
    Bomb bomb = {(uint8_t)kind, 0, (int16_t)(alien->x + alien_width(alien) / 2 - 1),
                 (int16_t)(alien->y + ALIEN_HEIGHT)};
    return bomb;
}

/** Whether a new bomb may drop: the others have fallen far enough, and plungers stop with one
 * invader left. */
static int can_drop(const Bombs *bombs, BombKind kind, const BombContext *context)
{
    for (uint32_t i = 0; i < bombs->count; i++)
        if (bombs->active[i].steps < reload_steps(context->score))
            return 0;
    return !(kind == BOMB_PLUNGER && context->fleet->count <= 1u);
}

/** The column the bomb of `kind` comes from (0: none), moving on in its table. */
static uint32_t pick_column(Bombs *bombs, BombKind kind, const BombContext *context)
{
    uint32_t column = 0;
    switch (kind) {
    case BOMB_ROLLING:
        column = column_above(context->fleet, context->cannon_center_x);
        break;
    case BOMB_PLUNGER:
        column = COLUMN_FIRE_TABLE[PLUNGER_START + bombs->plunger_index];
        bombs->plunger_index = (uint8_t)((bombs->plunger_index + 1u) % PLUNGER_LENGTH);
        break;
    default:
        column = COLUMN_FIRE_TABLE[SQUIGGLY_START + bombs->squiggly_index];
        bombs->squiggly_index = (uint8_t)((bombs->squiggly_index + 1u) % SQUIGGLY_LENGTH);
        break;
    }
    return column;
}

/** Tries to drop a bomb of `kind` from the column chosen by its rule. */
static Bombs try_drop(const Bombs *bombs, BombKind kind, const BombContext *context)
{
    if (!can_drop(bombs, kind, context))
        return *bombs;
    Bombs next = *bombs;
    const uint32_t column = pick_column(&next, kind, context);
    const int32_t shooter = column ? lowest_in_column(context->fleet, column) : -1;
    if (shooter >= 0 && next.count < BOMB_KIND_COUNT)
        next.active[next.count++] = drop_from(kind, &context->fleet->aliens[shooter]);
    return next;
}

/** Index of the bomb of `kind` on screen, or -1. */
static int32_t find_kind(const Bombs *bombs, BombKind kind)
{
    for (uint32_t i = 0; i < bombs->count; i++)
        if (bombs->active[i].kind == kind)
            return (int32_t)i;
    return -1;
}

Bombs bombs_step(const Bombs *bombs, const BombContext *context)
{
    const BombKind kind = (BombKind)(bombs->turn % BOMB_KIND_COUNT);
    Bombs next = *bombs;
    next.turn++;
    const int32_t falling = find_kind(&next, kind);
    if (falling < 0)
        return try_drop(&next, kind, context);
    next.active[falling].y = (int16_t)(next.active[falling].y + bomb_speed(context->fleet->count));
    next.active[falling].steps++;
    return next;
}

Bombs bombs_remove(const Bombs *bombs, uint32_t index)
{
    if (index >= bombs->count)
        return *bombs;
    Bombs next = *bombs;
    for (uint32_t i = index; i + 1u < bombs->count; i++)
        next.active[i] = bombs->active[i + 1u];
    next.count--;
    return next;
}
