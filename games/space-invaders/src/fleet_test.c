/* SPDX-License-Identifier: GPL-3.0-only */
#include "fleet.h"
#include "test/check.h"

/** A fleet with only the first `count` invaders of the formation. */
static Fleet fleet_of(uint32_t count)
{
    Fleet fleet = fleet_create(1);
    fleet.count = (uint8_t)count;
    return fleet;
}

/** The fleet after `frames` frames. */
static Fleet run(Fleet fleet, uint32_t frames)
{
    for (uint32_t i = 0; i < frames; i++)
        fleet = fleet_step(&fleet);
    return fleet;
}

static void it_moves_exactly_one_invader_per_frame(void)
{
    const Fleet start = fleet_create(1);
    const Fleet next = fleet_step(&start);
    CHECK_EQ(next.aliens[0].x, start.aliens[0].x + STEP_X);
    CHECK_EQ(next.aliens[0].frame, 1);
    CHECK_EQ(next.aliens[1].x, start.aliens[1].x);
    CHECK_EQ(next.cursor, 1);
}

static void it_completes_a_pass_in_as_many_frames_as_there_are_invaders(void)
{
    const Fleet start = fleet_create(1);
    const Fleet pass = run(start, 55);
    CHECK_EQ(pass.cursor, 0);
    for (uint32_t i = 0; i < 55; i++)
        CHECK_EQ(pass.aliens[i].x, start.aliens[i].x + STEP_X);
}

static void it_gets_faster_one_invader_left_moves_every_frame(void)
{
    const Fleet lonely = fleet_of(1);
    const Fleet later = run(lonely, 2);
    CHECK_EQ(later.aliens[0].x, lonely.aliens[0].x + 2 * LAST_ALIEN_STEP_RIGHT);
}

static void it_moves_the_last_invader_3_pixels_right_but_only_2_left(void)
{
    Fleet lonely = fleet_of(1);
    CHECK_EQ(fleet_step(&lonely).aliens[0].x, lonely.aliens[0].x + 3);
    lonely.direction = -1;
    CHECK_EQ(fleet_step(&lonely).aliens[0].x, lonely.aliens[0].x - 2);
}

static void it_drops_and_turns_around_after_reaching_the_edge(void)
{
    Fleet fleet = fleet_of(1);
    fleet.aliens[0].x = FLEET_RIGHT_LIMIT - 12 - 3;
    fleet = fleet_step(&fleet); /* reaches the edge: the next pass drops */
    CHECK(fleet.dropping);
    CHECK_EQ(fleet.direction, -1);
    const int32_t y = fleet.aliens[0].y;
    fleet = fleet_step(&fleet);
    CHECK_EQ(fleet.aliens[0].y, y + STEP_DOWN);
    CHECK(!fleet.dropping);
}

static void removing_keeps_the_cursor_on_the_invader_due_to_move_next(void)
{
    const Fleet start = run(fleet_create(1), 5);
    const Fleet fewer = fleet_remove(&start, 2);
    CHECK_EQ(fewer.count, 54);
    CHECK_EQ(fewer.cursor, 4);
    CHECK_EQ(fewer.aliens[fewer.cursor].column, start.aliens[start.cursor].column);
    const Fleet later = fleet_remove(&start, 30);
    CHECK_EQ(later.cursor, 5);
}

static void removing_the_last_invader_due_to_move_ends_the_pass(void)
{
    const Fleet start = run(fleet_create(1), 54);
    const Fleet fewer = fleet_remove(&start, 54);
    CHECK_EQ(fewer.cursor, 0);
    CHECK_EQ(fewer.beat, 1u);
}

static void the_bottom_is_the_lowest_edge_of_the_formation(void)
{
    const Fleet fleet = fleet_create(1);
    CHECK_EQ(fleet_bottom(&fleet), FIRST_ROUND_BOTTOM_Y + ALIEN_HEIGHT);
}

int main(void)
{
    RUN(it_moves_exactly_one_invader_per_frame);
    RUN(it_completes_a_pass_in_as_many_frames_as_there_are_invaders);
    RUN(it_gets_faster_one_invader_left_moves_every_frame);
    RUN(it_moves_the_last_invader_3_pixels_right_but_only_2_left);
    RUN(it_drops_and_turns_around_after_reaching_the_edge);
    RUN(removing_keeps_the_cursor_on_the_invader_due_to_move_next);
    RUN(removing_the_last_invader_due_to_move_ends_the_pass);
    RUN(the_bottom_is_the_lowest_edge_of_the_formation);
    return check_report();
}
