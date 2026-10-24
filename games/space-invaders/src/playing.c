/* SPDX-License-Identifier: GPL-3.0-only */
#include "playing.h"

#include "collisions.h"

PlayingState playing_initial(uint32_t round)
{
    PlayingState state;
    state.round = round;
    state.score = 0;
    state.lives = STARTING_LIVES;
    state.extra_life_awarded = 0;
    state.shots_fired = 0;
    state.cannon_x = CANNON_MIN_X;
    state.cannon_explosion = 0;
    state.shot = NO_SHOT;
    state.fleet = fleet_create(round);
    state.bombs = bombs_none();
    state.shields = shields_create();
    state.ufo = ufo_initial();
    state.effects = effects_none();
    return state;
}

int playing_is_over(const PlayingState *state)
{
    return (state->lives == 0 && state->cannon_explosion == 0) ||
           fleet_bottom(&state->fleet) >= CANNON_Y;
}

/** Moves the cannon, then moves its shot or fires a new one if fire was pressed. */
static PlayingState advance_cannon(const PlayingState *state, const Controls *controls)
{
    PlayingState next = *state;
    next.cannon_x = cannon_move(state->cannon_x, controls->direction);
    if (state->shot.active) {
        next.shot = shot_move(&state->shot);
    } else if (controls->fire) {
        next.shot = shot_fire(next.cannon_x);
        next.shots_fired++;
    }
    return next;
}

/** Moves everything that moves by itself: invaders, bombs, the mystery ship and the effects. */
static PlayingState advance_world(const PlayingState *state)
{
    /* bombs and ship see the formation as it was before this frame's step, as in the web version */
    const BombContext context = {&state->fleet, cannon_center(state->cannon_x), state->score};
    PlayingState next = *state;
    next.fleet = fleet_step(&state->fleet);
    next.bombs = bombs_step(&state->bombs, &context);
    next.ufo = ufo_step(&state->ufo, state->fleet.count, state->shots_fired);
    next.effects = effects_step(&state->effects);
    return next;
}

/** Starts the next round once every invader is destroyed: lower formation, new shields. */
static PlayingState next_round_if_cleared(const PlayingState *state)
{
    if (state->fleet.count > 0)
        return *state;
    PlayingState next = *state;
    next.round = state->round + 1u;
    next.fleet = fleet_create(next.round);
    next.shields = shields_create();
    next.bombs = bombs_none();
    next.shot = NO_SHOT;
    return next;
}

/** One frame while the cannon explodes: everything else waits, then a new cannon arrives. */
static PlayingState step_cannon_explosion(const PlayingState *state)
{
    PlayingState next = *state;
    if (state->cannon_explosion > 1u) {
        next.cannon_explosion--;
        next.effects = effects_step(&state->effects);
        return next;
    }
    next.cannon_explosion = 0;
    next.cannon_x = CANNON_MIN_X;
    next.bombs = bombs_none();
    next.shot = NO_SHOT;
    return next;
}

/** A collision rule: the state with one kind of collision resolved. */
typedef PlayingState (*Rule)(const PlayingState *state);

/** The collision rules, applied in order after everything has moved. */
static const Rule COLLISION_RULES[] = {
    shot_hits_alien,  shot_hits_ufo,         shot_hits_bomb,   shot_hits_shield,
    bombs_hit_cannon, bombs_hit_shields,     bombs_hit_ground, aliens_crush_shields,
    award_extra_life, next_round_if_cleared,
};

PlayingState playing_step(const PlayingState *state, const Controls *controls)
{
    if (state->cannon_explosion)
        return step_cannon_explosion(state);
    const PlayingState moved = advance_cannon(state, controls);
    PlayingState current = advance_world(&moved);
    for (uint32_t i = 0; i < sizeof COLLISION_RULES / sizeof COLLISION_RULES[0]; i++)
        current = COLLISION_RULES[i](&current);
    return current;
}
