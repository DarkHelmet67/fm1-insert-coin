/* SPDX-License-Identifier: GPL-3.0-only */
#include "beam.h"

#include "palette.h"
#include "rom.h"
#include "table.h"
#include "tiles.h"

/** Width of the beam in cells, and the screen row of its first row. */
#define BEAM_WIDTH 6
#define FIRST_ROW 23

int32_t beam_column(int32_t beam_x)
{
    return 29 - (((0x10 - beam_x) & 0xff) >> 3);
}

int32_t beam_palette(int32_t frame)
{
    int32_t step = (frame >> 2) & 3;
    return 0x17 + (step == 0 ? 1 : step);
}

void draw_beam(Framebuffer *fb, const Capture *capture, int32_t frame)
{
    const TileColors colors = char_colors(beam_palette(frame));
    int32_t left = beam_column(capture->beam_x);
    for (int32_t row = 0; row < BEAM_ROWS; row++) {
        if (!beam_row(capture, row + 1))
            continue;
        for (int32_t i = 0; i < BEAM_WIDTH; i++)
            draw_char(fb, table_at(BEAM_TILES, BEAM_TILES_SIZE, row * BEAM_WIDTH + i, 0x24),
                      left + i, FIRST_ROW + row, &colors);
    }
}
