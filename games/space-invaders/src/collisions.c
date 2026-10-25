/* SPDX-License-Identifier: GPL-3.0-only */
#include "collisions.h"

#include "sprites.h"

/** A bitmap placed on the screen. */
static Placed place(const Bitmap *bitmap, int32_t x, int32_t y)
{
    Placed placed = {bitmap, x, y};
    return placed;
}

/** The pixels of an invader in its current animation frame. */
static Bitmap alien_bitmap(const Alien *alien)
{
    return bitmap_from_sprite(alien_sprite((AlienKind)alien->kind, alien->frame));
}

/** The pixels of a bomb in its current animation frame. */
static Bitmap bomb_bitmap(const Bomb *bomb)
{
    return bitmap_from_sprite(bomb_sprite((BombKind)bomb->kind, bomb->steps));
}

/** Whether the shot touches the bitmap placed at (x, y). */
static int shot_touches(const Shot *shot, const Bitmap *bitmap, int32_t x, int32_t y)
{
    const Bitmap shot_bitmap = bitmap_from_sprite(&SHOT_SPRITE);
    const Placed a = place(&shot_bitmap, shot->x, shot->y);
    const Placed b = place(bitmap, x, y);
    return placed_overlap(&a, &b);
}

PlayingState shot_hits_alien(const PlayingState *state)
{
    if (!state->shot.active)
        return *state;
    for (uint32_t i = 0; i < state->fleet.count; i++) {
        const Alien *alien = &state->fleet.aliens[i];
        const Bitmap bitmap = alien_bitmap(alien);
        if (shot_touches(&state->shot, &bitmap, alien->x, alien->y)) {
            PlayingState next = *state;
            next.shot = NO_SHOT;
            next.fleet = fleet_remove(&state->fleet, i);
            next.effects = effects_add(&state->effects, effect_explode_alien(alien));
            next.score = state->score + alien_points(alien);
            return next;
        }
    }
    return *state;
}

PlayingState shot_hits_ufo(const PlayingState *state)
{
    if (!state->shot.active || !state->ufo.flying)
        return *state;
    const Bitmap ufo = bitmap_from_sprite(&UFO_SPRITE);
    if (!shot_touches(&state->shot, &ufo, state->ufo.x, UFO_Y))
        return *state;
    const int32_t points = ufo_score(state->shots_fired);
    PlayingState next = *state;
    next.shot = NO_SHOT;
    next.ufo = ufo_shot_down(&state->ufo);
    next.effects =
        effects_add(&state->effects, effect_create(EFFECT_UFO, state->ufo.x, UFO_Y, points));
    next.score = state->score + points;
    return next;
}

PlayingState shot_hits_bomb(const PlayingState *state)
{
    if (!state->shot.active)
        return *state;
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        const Bomb *bomb = &state->bombs.active[i];
        const Bitmap bitmap = bomb_bitmap(bomb);
        if (shot_touches(&state->shot, &bitmap, bomb->x, bomb->y)) {
            const Effect splash =
                effect_create(EFFECT_SHOT, state->shot.x - 3, state->shot.y - 2, NO_POINTS);
            PlayingState next = *state;
            next.shot = NO_SHOT;
            next.bombs = bombs_remove(&state->bombs, i);
            next.effects = effects_add(&state->effects, splash);
            return next;
        }
    }
    return *state;
}

PlayingState shot_hits_shield(const PlayingState *state)
{
    if (!state->shot.active)
        return *state;
    const Bitmap shot_bitmap = bitmap_from_sprite(&SHOT_SPRITE);
    const Placed shot = place(&shot_bitmap, state->shot.x, state->shot.y);
    if (!shields_hit(&state->shields, &shot))
        return *state;
    /* the splash is centred on the tip of the shot */
    const Bitmap splash_bitmap = bitmap_from_sprite(&SHOT_EXPLOSION_SPRITE);
    const Placed splash = place(&splash_bitmap, state->shot.x - 3, state->shot.y - 2);
    PlayingState next = *state;
    next.shot = NO_SHOT;
    next.shields = shields_damage(&state->shields, &splash);
    return next;
}

PlayingState bombs_hit_cannon(const PlayingState *state)
{
    const Bitmap cannon_bitmap = bitmap_from_sprite(&CANNON_SPRITE);
    const Placed cannon = place(&cannon_bitmap, state->cannon_x, CANNON_Y);
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        const Bitmap bitmap = bomb_bitmap(&state->bombs.active[i]);
        const Placed bomb = place(&bitmap, state->bombs.active[i].x, state->bombs.active[i].y);
        if (placed_overlap(&bomb, &cannon)) {
            PlayingState next = *state;
            next.lives = state->lives - 1;
            next.cannon_explosion = CANNON_EXPLOSION_FRAMES;
            next.bombs = bombs_remove(&state->bombs, i);
            return next;
        }
    }
    return *state;
}

PlayingState bombs_hit_shields(const PlayingState *state)
{
    /* every bomb is tested against the shields as they were before this rule */
    uint8_t hit[BOMB_KIND_COUNT] = {0};
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        const Bitmap bitmap = bomb_bitmap(&state->bombs.active[i]);
        const Placed bomb = place(&bitmap, state->bombs.active[i].x, state->bombs.active[i].y);
        hit[i] = (uint8_t)shields_hit(&state->shields, &bomb);
    }
    PlayingState next = *state;
    const Bitmap splash_bitmap = bitmap_from_sprite(&BOMB_EXPLOSION_SPRITE);
    for (uint32_t i = state->bombs.count; i-- > 0;) {
        if (!hit[i])
            continue;
        next.bombs = bombs_remove(&next.bombs, i);
    }
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        if (!hit[i])
            continue;
        /* the splash goes around the lower end of the bomb */
        const Bomb *bomb = &state->bombs.active[i];
        const Placed splash = place(&splash_bitmap, bomb->x - 1, bomb->y + 2);
        next.shields = shields_damage(&next.shields, &splash);
    }
    return next;
}

PlayingState bombs_hit_ground(const PlayingState *state)
{
    PlayingState next = *state;
    for (uint32_t i = state->bombs.count; i-- > 0;)
        if (state->bombs.active[i].y + 8 >= GROUND_Y)
            next.bombs = bombs_remove(&next.bombs, i);
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        const Bomb *bomb = &state->bombs.active[i];
        if (bomb->y + 8 >= GROUND_Y)
            next.effects = effects_add(
                &next.effects, effect_create(EFFECT_BOMB, bomb->x - 1, GROUND_Y - 8, NO_POINTS));
    }
    return next;
}

PlayingState aliens_crush_shields(const PlayingState *state)
{
    PlayingState next = *state;
    for (uint32_t i = 0; i < state->fleet.count; i++) {
        const Alien *alien = &state->fleet.aliens[i];
        /* a quick test first: invaders above the shields cannot touch them */
        if (alien->y + ALIEN_HEIGHT <= SHIELD_Y)
            continue;
        const Bitmap bitmap = alien_bitmap(alien);
        const Placed placed = place(&bitmap, alien->x, alien->y);
        next.shields = shields_damage(&next.shields, &placed);
    }
    return next;
}

PlayingState award_extra_life(const PlayingState *state)
{
    if (state->extra_life_awarded || state->score < EXTRA_LIFE_SCORE)
        return *state;
    PlayingState next = *state;
    next.lives = state->lives + 1;
    next.extra_life_awarded = 1;
    return next;
}
