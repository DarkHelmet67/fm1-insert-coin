/* SPDX-License-Identifier: GPL-3.0-only */
#include "playing.h"
#include "test/check.h"

static const Controls IDLE = {0, 0, 0};
static const Controls FIRE = {0, 1, 0};

/** PlayingState is about 1 KB: the tests keep theirs out of the stack. */
static PlayingState state, next;

static void it_fires_a_shot_and_counts_it(void)
{
    state = playing_initial(1);
    next = playing_step(&state, &FIRE);
    CHECK(next.shot.active);
    CHECK_EQ(next.shots_fired, 1u);
}

static void it_allows_only_one_shot_at_a_time(void)
{
    state = playing_initial(1);
    next = playing_step(&state, &FIRE);
    const int32_t y = next.shot.y;
    next = playing_step(&next, &FIRE);
    CHECK_EQ(next.shots_fired, 1u);
    CHECK_EQ(next.shot.y, y - SHOT_SPEED);
}

static void it_brings_a_lower_formation_when_the_last_invader_is_destroyed(void)
{
    state = playing_initial(1);
    state.fleet.count = 1;
    state.fleet.aliens[0] = formation_alien(0, 1);
    const Shot shot = {1, (int16_t)(state.fleet.aliens[0].x + 5),
                       (int16_t)(state.fleet.aliens[0].y + 8)};
    state.shot = shot;
    next = playing_step(&state, &IDLE);
    CHECK_EQ(next.round, 2u);
    CHECK_EQ(next.fleet.count, FORMATION_SIZE);
    CHECK_EQ(next.fleet.aliens[0].y, start_bottom_y(2));
}

static void it_freezes_the_game_while_the_cannon_explodes(void)
{
    state = playing_initial(1);
    state.cannon_explosion = 2;
    state.cannon_x = 100;
    next = playing_step(&state, &FIRE);
    CHECK_EQ(next.cannon_explosion, 1u);
    CHECK_EQ(next.fleet.cursor, state.fleet.cursor);
    CHECK(!next.shot.active);
    next = playing_step(&next, &IDLE);
    CHECK_EQ(next.cannon_explosion, 0u);
    CHECK_EQ(next.cannon_x, CANNON_MIN_X);
}

static void it_is_over_once_the_last_cannon_has_finished_exploding(void)
{
    state = playing_initial(1);
    state.lives = 0;
    state.cannon_explosion = 5;
    CHECK(!playing_is_over(&state));
    state.cannon_explosion = 0;
    CHECK(playing_is_over(&state));
}

static void it_is_over_when_the_invaders_reach_the_cannon(void)
{
    state = playing_initial(1);
    CHECK(!playing_is_over(&state));
    state.fleet.aliens[0].y = CANNON_Y - ALIEN_HEIGHT;
    CHECK(playing_is_over(&state));
}

int main(void)
{
    RUN(it_fires_a_shot_and_counts_it);
    RUN(it_allows_only_one_shot_at_a_time);
    RUN(it_brings_a_lower_formation_when_the_last_invader_is_destroyed);
    RUN(it_freezes_the_game_while_the_cannon_explodes);
    RUN(it_is_over_once_the_last_cannon_has_finished_exploding);
    RUN(it_is_over_when_the_invaders_reach_the_cannon);
    return check_report();
}
