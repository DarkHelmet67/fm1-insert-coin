/* SPDX-License-Identifier: GPL-3.0-only */
#include "screen/screen_map.h"
#include "test/check.h"

static void a_smaller_screen_is_centred(void)
{
    /* Breakout, 228 x 208 */
    const ScreenMap map = screen_map_centered(228u, 208u);
    CHECK_EQ(map.dest_x, 6u);
    CHECK_EQ(map.source_x, 0u);
    CHECK_EQ(map.width, 228u);
    CHECK_EQ(map.source_row[15], SCREEN_NO_ROW);
    CHECK_EQ(map.source_row[16], 0);
    CHECK_EQ(map.source_row[223], 207);
    CHECK_EQ(map.source_row[224], SCREEN_NO_ROW);
}

static void a_taller_screen_is_cropped_evenly(void)
{
    /* Galaga, 224 x 288: 24 rows lost at the top, 24 at the bottom */
    const ScreenMap map = screen_map_centered(224u, 288u);
    CHECK_EQ(map.source_row[0], 24);
    CHECK_EQ(map.source_row[239], 263);
}

static void a_wider_screen_is_cropped_evenly(void)
{
    const ScreenMap map = screen_map_centered(256u, 100u);
    CHECK_EQ(map.width, 240u);
    CHECK_EQ(map.source_x, 8u);
    CHECK_EQ(map.dest_x, 0u);
}

static void runs_of_rows_are_stacked(void)
{
    /* Space Invaders: the empty top 8 rows go, and the gap under the ground line */
    const RowRange rows[] = {{8u, 239u}, {243u, 250u}};
    const ScreenMap map = screen_map_rows(224u, rows, 2u);
    CHECK_EQ(map.source_row[0], 8);
    CHECK_EQ(map.source_row[231], 239);
    CHECK_EQ(map.source_row[232], 243);
    CHECK_EQ(map.source_row[239], 250);
    CHECK_EQ(map.dest_x, 8u);
}

static void short_runs_are_centred(void)
{
    const RowRange rows[] = {{0u, 9u}, {20u, 29u}};
    const ScreenMap map = screen_map_rows(224u, rows, 2u);
    CHECK_EQ(map.source_row[109], SCREEN_NO_ROW);
    CHECK_EQ(map.source_row[110], 0);
    CHECK_EQ(map.source_row[120], 20);
    CHECK_EQ(map.source_row[130], SCREEN_NO_ROW);
}

static void rows_past_the_bottom_are_dropped(void)
{
    const RowRange rows[] = {{0u, 299u}};
    const ScreenMap map = screen_map_rows(224u, rows, 1u);
    CHECK_EQ(map.source_row[0], 0);
    CHECK_EQ(map.source_row[239], 239);
}

int main(void)
{
    RUN(a_smaller_screen_is_centred);
    RUN(a_taller_screen_is_cropped_evenly);
    RUN(a_wider_screen_is_cropped_evenly);
    RUN(runs_of_rows_are_stacked);
    RUN(short_runs_are_centred);
    RUN(rows_past_the_bottom_are_dropped);
    return check_report();
}
