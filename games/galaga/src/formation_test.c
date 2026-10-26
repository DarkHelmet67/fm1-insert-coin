/* SPDX-License-Identifier: GPL-3.0-only */
#include "formation.h"
#include "test/check.h"

static void the_grid_starts_at_the_origins_rows_as_sprite_heights(void)
{
    const Formation f = initial_formation();
    CHECK_EQ(f.sprite_coords[0], 0x31);
    CHECK_EQ(f.sprite_coords[1], 0x41);
    CHECK_EQ(f.sprite_coords[10], ((~(0x92 + 0x4f) & 0xff) << 1) & 0x1ff);
}

static void it_sways_one_pixel_every_4_frames_and_turns_back_at_32(void)
{
    Formation f = initial_formation();
    for (int32_t frame = 1; frame <= 33 * 4; frame++)
        f = sway_formation(&f, frame, 0).formation;
    CHECK_EQ(f.offsets[0], 31);
    CHECK_EQ(f.direction, 1);
}

static void it_stops_in_the_middle_once_asked_and_then_breathes(void)
{
    const Formation start = initial_formation();
    const SwayResult result = sway_formation(&start, 1, 1);
    CHECK(!result.settled);
    Formation turned = result.formation;
    turned.direction = 1;
    const SwayResult back = sway_formation(&turned, 5, 1);
    CHECK(back.settled);
    const Formation breathed = breathe_formation(&back.formation, 4);
    /* the outer columns move every time with bitmap $FF: the left one out, the right one out */
    CHECK_EQ(breathed.offsets[0], 0xff);
    CHECK_EQ(breathed.offsets[9], 1);
    CHECK_EQ(breathed.direction, 1);
}

static void out_of_range_cells_read_as_zero(void)
{
    const Formation f = initial_formation();
    CHECK_EQ(formation_offset(&f, CELLS), 0);
    CHECK_EQ(formation_coord(&f, -1), 0);
}

int main(void)
{
    RUN(the_grid_starts_at_the_origins_rows_as_sprite_heights);
    RUN(it_sways_one_pixel_every_4_frames_and_turns_back_at_32);
    RUN(it_stops_in_the_middle_once_asked_and_then_breathes);
    RUN(out_of_range_cells_read_as_zero);
    return check_report();
}
