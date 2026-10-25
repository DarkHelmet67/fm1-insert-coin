/* SPDX-License-Identifier: GPL-3.0-only */
#include "collisions.h"
#include "test/check.h"

/** A new game with the shot at (x, y). */
static PlayingState with_shot(int32_t x, int32_t y)
{
    PlayingState state = playing_initial(1);
    const Shot shot = {1, (int16_t)x, (int16_t)y};
    state.shot = shot;
    return state;
}

/** A new game with one bomb of `kind` at (x, y). */
static PlayingState with_bomb(BombKind kind, int32_t x, int32_t y)
{
    PlayingState state = playing_initial(1);
    const Bomb bomb = {(uint8_t)kind, 1, (int16_t)x, (int16_t)y};
    state.bombs.active[0] = bomb;
    state.bombs.count = 1;
    return state;
}

static void the_shot_destroys_the_invader_hit_and_scores_its_points(void)
{
    const Alien target = formation_alien(0, 1); /* octopus, bottom left */
    const PlayingState state = with_shot(target.x + 5, target.y + 4);
    const PlayingState next = shot_hits_alien(&state);
    CHECK(!next.shot.active);
    CHECK_EQ(next.fleet.count, 54);
    CHECK_EQ(next.score, 10);
    CHECK_EQ(next.effects.count, 1);
    CHECK_EQ(next.effects.list[0].kind, EFFECT_ALIEN);
}

static void the_shot_misses_through_the_gaps_of_the_sprite(void)
{
    const Alien squid = formation_alien(44, 1);
    /* the squid's first column is empty in its top three rows */
    const PlayingState state = with_shot(squid.x, squid.y - 1);
    const PlayingState next = shot_hits_alien(&state);
    CHECK(next.shot.active);
    CHECK_EQ(next.fleet.count, 55);
}

static void the_mystery_ship_scores_by_the_number_of_shots_fired(void)
{
    PlayingState state = with_shot(UFO_LEFT_X + 8, UFO_Y + 2);
    state.ufo.flying = 1;
    state.ufo.x = UFO_LEFT_X;
    state.shots_fired = 23;
    const PlayingState next = shot_hits_ufo(&state);
    CHECK_EQ(next.score, 300);
    CHECK(!next.ufo.flying);
    CHECK_EQ(next.effects.list[0].points, 300);
}

static void the_shot_stops_at_a_shield_and_carves_it(void)
{
    const PlayingState state = with_shot(FIRST_SHIELD_X + 10, SHIELD_Y + 2);
    const PlayingState next = shot_hits_shield(&state);
    CHECK(!next.shot.active);
    CHECK(bitmap_count(&next.shields.list[0].bitmap) < bitmap_count(&state.shields.list[0].bitmap));
}

static void a_bomb_destroys_the_cannon_and_costs_a_life(void)
{
    const PlayingState state = with_bomb(BOMB_PLUNGER, CANNON_MIN_X + 5, CANNON_Y - 4);
    const PlayingState next = bombs_hit_cannon(&state);
    CHECK_EQ(next.lives, STARTING_LIVES - 1);
    CHECK_EQ(next.cannon_explosion, CANNON_EXPLOSION_FRAMES);
    CHECK_EQ(next.bombs.count, 0);
}

static void a_bomb_carves_the_shields(void)
{
    const PlayingState state = with_bomb(BOMB_ROLLING, FIRST_SHIELD_X + 9, SHIELD_Y - 4);
    const PlayingState next = bombs_hit_shields(&state);
    CHECK_EQ(next.bombs.count, 0);
    CHECK(bitmap_count(&next.shields.list[0].bitmap) < bitmap_count(&state.shields.list[0].bitmap));
}

static void a_bomb_explodes_on_the_ground(void)
{
    const PlayingState state = with_bomb(BOMB_SQUIGGLY, 120, GROUND_Y - 8);
    const PlayingState next = bombs_hit_ground(&state);
    CHECK_EQ(next.bombs.count, 0);
    CHECK_EQ(next.effects.count, 1);
    CHECK_EQ(next.effects.list[0].x, 119);
    CHECK_EQ(next.effects.list[0].y, GROUND_Y - 8);
}

static void invaders_erase_the_shields_they_walk_over(void)
{
    PlayingState state = playing_initial(1);
    state.fleet.aliens[0].x = FIRST_SHIELD_X;
    state.fleet.aliens[0].y = SHIELD_Y + 4;
    const PlayingState next = aliens_crush_shields(&state);
    CHECK(bitmap_count(&next.shields.list[0].bitmap) < bitmap_count(&state.shields.list[0].bitmap));
    CHECK_EQ(bitmap_count(&next.shields.list[1].bitmap),
             bitmap_count(&state.shields.list[1].bitmap));
}

static void one_extra_cannon_at_1500_points_once(void)
{
    PlayingState state = playing_initial(1);
    state.score = EXTRA_LIFE_SCORE;
    const PlayingState once = award_extra_life(&state);
    CHECK_EQ(once.lives, STARTING_LIVES + 1);
    const PlayingState twice = award_extra_life(&once);
    CHECK_EQ(twice.lives, STARTING_LIVES + 1);
}

int main(void)
{
    RUN(the_shot_destroys_the_invader_hit_and_scores_its_points);
    RUN(the_shot_misses_through_the_gaps_of_the_sprite);
    RUN(the_mystery_ship_scores_by_the_number_of_shots_fired);
    RUN(the_shot_stops_at_a_shield_and_carves_it);
    RUN(a_bomb_destroys_the_cannon_and_costs_a_life);
    RUN(a_bomb_carves_the_shields);
    RUN(a_bomb_explodes_on_the_ground);
    RUN(invaders_erase_the_shields_they_walk_over);
    RUN(one_extra_cannon_at_1500_points_once);
    return check_report();
}
