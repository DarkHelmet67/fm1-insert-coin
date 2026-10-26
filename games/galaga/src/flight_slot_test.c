/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_slot.h"
#include "test/check.h"

static void near_vertical_the_upright_drawing_is_used(void)
{
    const Pose up = pose_for(0x100);
    const Pose down = pose_for(0x300);
    CHECK_EQ(up.tile, 6);
    CHECK(!up.flip_x && !up.flip_y);
    CHECK_EQ(down.tile, 6);
    CHECK(down.flip_x && down.flip_y);
}

static void pointing_right_the_horizontal_drawing_is_mirrored(void)
{
    const Pose right = pose_for(0x000);
    CHECK_EQ(right.tile, 0);
    CHECK(right.flip_x && !right.flip_y);
    CHECK_EQ(pose_for(0x200).tile, 0);
}

static void a_quarter_turn_goes_through_six_drawings(void)
{
    static const int32_t ANGLES[6] = {0x00, 0x30, 0x60, 0x90, 0xc0, 0xe0};
    for (int32_t i = 0; i < 6; i++)
        CHECK_EQ(pose_for(ANGLES[i]).tile, i);
}

static void the_whole_speed_goes_along_the_main_axis(void)
{
    const FlightPoint right = step_position(0x4000, 0x4000, 0x000, 2);
    const FlightPoint down = step_position(0x4000, 0x4000, 0x300, 3);
    CHECK_EQ(right.x, 0x4100);
    CHECK_EQ(right.y, 0x4000);
    CHECK_EQ(down.x, 0x4000);
    CHECK_EQ(down.y, 0x3e80);
}

static void a_diagonal_moves_almost_equally_on_both_axes(void)
{
    const FlightPoint moved = step_position(0x4000, 0x4000, 0x080, 2);
    CHECK_EQ(moved.y - 0x4000, 256);
    CHECK_EQ(moved.x - 0x4000, 254);
}

static void speed_0_does_not_move(void)
{
    const FlightPoint still = step_position(1, 2, 0x123, 0);
    CHECK_EQ(still.x, 1);
    CHECK_EQ(still.y, 2);
}

static void flight_coordinates_become_sprite_registers(void)
{
    FlightSlot slot = new_slot(8);
    slot.y = 312 << 7; /* the top of the screen: sprite y 41, just below the scores' 40 */
    slot.x = 100 << 7;
    const SpritePoint at = sprite_position(&slot);
    CHECK_EQ(at.x, 100);
    CHECK_EQ(at.y, 41);
}

static void heading_home_adds_the_frozen_formation_offset(void)
{
    FlightSlot slot = new_slot(8);
    slot.y = 300 << 7;
    slot.x = 100 << 7;
    slot.homing = 1;
    slot.offset_x = 0xfe;
    CHECK_EQ(sprite_position(&slot).x, 98);
}

int main(void)
{
    RUN(near_vertical_the_upright_drawing_is_used);
    RUN(pointing_right_the_horizontal_drawing_is_mirrored);
    RUN(a_quarter_turn_goes_through_six_drawings);
    RUN(the_whole_speed_goes_along_the_main_axis);
    RUN(a_diagonal_moves_almost_equally_on_both_axes);
    RUN(speed_0_does_not_move);
    RUN(flight_coordinates_become_sprite_registers);
    RUN(heading_home_adds_the_frozen_formation_offset);
    return check_report();
}
