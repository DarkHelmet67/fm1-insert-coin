/* SPDX-License-Identifier: GPL-3.0-only */
#include "bombs.h"
#include "cannon.h"
#include "test/check.h"

static void it_reloads_faster_as_the_score_grows(void)
{
    CHECK_EQ(reload_steps(0), 48);
    CHECK_EQ(reload_steps(200), 16);
    CHECK_EQ(reload_steps(1000), 11);
    CHECK_EQ(reload_steps(2000), 8);
    CHECK_EQ(reload_steps(3000), 7);
}

static void it_falls_faster_when_8_invaders_or_fewer_are_left(void)
{
    CHECK_EQ(bomb_speed(9), 4);
    CHECK_EQ(bomb_speed(8), 5);
}

static void it_finds_the_invader_that_drops_the_bombs_of_a_column(void)
{
    const Fleet fleet = fleet_create(1);
    CHECK_EQ(lowest_in_column(&fleet, 3), 2); /* bottom row, third column */
    Fleet thinner = fleet_remove(&fleet, 2);
    CHECK_EQ(thinner.aliens[lowest_in_column(&thinner, 3)].row, 4);
}

static void it_finds_the_column_above_the_cannon(void)
{
    const Fleet fleet = fleet_create(1);
    CHECK_EQ(column_above(&fleet, FORMATION_LEFT + 1), 1u);
    CHECK_EQ(column_above(&fleet, FORMATION_LEFT + CELL_SIZE + 5), 2u);
    CHECK_EQ(column_above(&fleet, 4), 0u);
}

static void it_drops_the_rolling_bomb_from_the_lowest_invader_above_the_cannon(void)
{
    const Fleet fleet = fleet_create(1);
    const BombContext context = {&fleet, FORMATION_LEFT + 4, 0};
    const Bombs none = bombs_none();
    const Bombs bombs = bombs_step(&none, &context);
    CHECK_EQ(bombs.count, 1);
    CHECK_EQ(bombs.active[0].kind, BOMB_ROLLING);
    CHECK_EQ(bombs.active[0].x, FORMATION_LEFT + 6 - 1);
    CHECK_EQ(bombs.active[0].y, FIRST_ROUND_BOTTOM_Y + ALIEN_HEIGHT);
}

static void it_moves_each_bomb_once_every_3_frames(void)
{
    const Fleet fleet = fleet_create(1);
    const BombContext context = {&fleet, FORMATION_LEFT + 4, 0};
    Bombs bombs = bombs_none();
    bombs = bombs_step(&bombs, &context);
    const int32_t y = bombs.active[0].y;
    for (int i = 0; i < 3; i++)
        bombs = bombs_step(&bombs, &context);
    CHECK_EQ(bombs.active[0].y, y + 4);
    CHECK_EQ(bombs.active[0].steps, 1);
}

static void it_waits_for_the_reload_distance_before_dropping_another(void)
{
    const Fleet fleet = fleet_create(1);
    const BombContext context = {&fleet, FORMATION_LEFT + 4, 0};
    Bombs bombs = bombs_none();
    for (uint32_t frame = 0; frame < 3u * 47u; frame++)
        bombs = bombs_step(&bombs, &context);
    CHECK_EQ(bombs.count, 1);
    for (uint32_t frame = 0; frame < 6; frame++)
        bombs = bombs_step(&bombs, &context);
    CHECK_EQ(bombs.count, 2);
}

static void it_never_drops_a_plunger_bomb_when_only_one_invader_is_left(void)
{
    Fleet fleet = fleet_create(1);
    fleet.count = 1;
    const BombContext context = {&fleet, 0, 0};
    Bombs bombs = bombs_none();
    bombs.turn = BOMB_PLUNGER;
    bombs = bombs_step(&bombs, &context);
    CHECK_EQ(bombs.count, 0);
}

static void removing_a_bomb_keeps_the_others_in_order(void)
{
    Bombs bombs = bombs_none();
    const Bomb a = {BOMB_ROLLING, 0, 1, 1}, b = {BOMB_PLUNGER, 0, 2, 2},
               c = {BOMB_SQUIGGLY, 0, 3, 3};
    bombs.active[0] = a;
    bombs.active[1] = b;
    bombs.active[2] = c;
    bombs.count = 3;
    const Bombs fewer = bombs_remove(&bombs, 1);
    CHECK_EQ(fewer.count, 2);
    CHECK_EQ(fewer.active[0].kind, BOMB_ROLLING);
    CHECK_EQ(fewer.active[1].kind, BOMB_SQUIGGLY);
}

int main(void)
{
    RUN(it_reloads_faster_as_the_score_grows);
    RUN(it_falls_faster_when_8_invaders_or_fewer_are_left);
    RUN(it_finds_the_invader_that_drops_the_bombs_of_a_column);
    RUN(it_finds_the_column_above_the_cannon);
    RUN(it_drops_the_rolling_bomb_from_the_lowest_invader_above_the_cannon);
    RUN(it_moves_each_bomb_once_every_3_frames);
    RUN(it_waits_for_the_reload_distance_before_dropping_another);
    RUN(it_never_drops_a_plunger_bomb_when_only_one_invader_is_left);
    RUN(removing_a_bomb_keeps_the_others_in_order);
    return check_report();
}
