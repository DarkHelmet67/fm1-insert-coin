/* SPDX-License-Identifier: GPL-3.0-only */
#include "aliens.h"
#include "test/check.h"

static void it_has_55_invaders_5_rows_of_11(void)
{
    CHECK_EQ(FORMATION_SIZE, 55);
    CHECK_EQ(formation_alien(54, 1).row, 1);
    CHECK_EQ(formation_alien(54, 1).column, 11);
}

static void it_lists_them_in_marching_order_bottom_row_first(void)
{
    const Alien first = formation_alien(0, 1);
    CHECK_EQ(first.kind, ALIEN_OCTOPUS);
    CHECK_EQ(first.row, 5);
    CHECK_EQ(first.column, 1);
    CHECK_EQ(first.y, FIRST_ROUND_BOTTOM_Y);
    CHECK_EQ(formation_alien(44, 1).kind, ALIEN_SQUID);
    CHECK_EQ(formation_alien(44, 1).y, FIRST_ROUND_BOTTOM_Y - 4 * CELL_SIZE);
}

static void it_centres_narrow_invaders_in_their_column(void)
{
    CHECK_EQ(formation_alien(0, 1).x, FORMATION_LEFT);      /* octopus: 12 wide */
    CHECK_EQ(formation_alien(22, 1).x, FORMATION_LEFT);     /* crab: 11 wide, half a pixel */
    CHECK_EQ(formation_alien(44, 1).x, FORMATION_LEFT + 2); /* squid: 8 wide */
}

static void it_starts_one_row_lower_in_round_2(void)
{
    CHECK_EQ(start_bottom_y(2), 152);
    CHECK_EQ(formation_alien(0, 2).y, 152);
}

static void it_never_starts_below_184_and_the_table_repeats(void)
{
    for (uint32_t round = 1; round < 30; round++)
        CHECK(start_bottom_y(round) <= 184);
    CHECK_EQ(start_bottom_y(10), 152);
}

static void each_kind_has_its_width_and_points(void)
{
    const Alien squid = formation_alien(44, 1);
    const Alien crab = formation_alien(22, 1);
    const Alien octopus = formation_alien(0, 1);
    CHECK_EQ(alien_width(&squid), 8);
    CHECK_EQ(alien_width(&crab), 11);
    CHECK_EQ(alien_width(&octopus), 12);
    CHECK_EQ(alien_points(&squid), 30);
    CHECK_EQ(alien_points(&crab), 20);
    CHECK_EQ(alien_points(&octopus), 10);
}

int main(void)
{
    RUN(it_has_55_invaders_5_rows_of_11);
    RUN(it_lists_them_in_marching_order_bottom_row_first);
    RUN(it_centres_narrow_invaders_in_their_column);
    RUN(it_starts_one_row_lower_in_round_2);
    RUN(it_never_starts_below_184_and_the_table_repeats);
    RUN(each_kind_has_its_width_and_points);
    return check_report();
}
