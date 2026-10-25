/* SPDX-License-Identifier: GPL-3.0-only */
#include "formation.h"

#include "rom.h"
#include "table.h"

/** Origin of each cell, as the program stores it. */
static const uint8_t CELL_ORIGINS[CELLS] = {0x31, 0x41, 0x51, 0x61, 0x71, 0x81, 0x91, 0xa1,
                                            0xb1, 0xc1, 0x92, 0x8a, 0x82, 0x7c, 0x76, 0x70};

/** Sprite y of a row origin: the "$0160 - 2n" of `c_12C3`. */
static uint16_t row_sprite_y(int32_t origin)
{
    return (uint16_t)(((~(origin + 0x4f) & 0xff) << 1) & 0x1ff);
}

Formation initial_formation(void)
{
    Formation f;
    for (int cell = 0; cell < CELLS; cell++) {
        f.offsets[cell] = 0;
        f.sprite_coords[cell] =
            cell < COLUMNS ? CELL_ORIGINS[cell] : row_sprite_y(CELL_ORIGINS[cell]);
        f.bitmap[cell] = 0;
    }
    f.direction = 0;
    return f;
}

int32_t cell_origin(int32_t cell)
{
    return cell >= 0 && cell < CELLS ? CELL_ORIGINS[cell] : 0;
}

/** The grid with cell `cell` moved by `step` (offset and sprite coordinate). */
static void move_cell(Formation *f, int cell, int32_t step)
{
    const int32_t mask = cell < COLUMNS ? 0xff : 0x1ff;
    f->offsets[cell] = (uint8_t)((f->offsets[cell] + step) & 0xff);
    f->sprite_coords[cell] = (uint16_t)((f->sprite_coords[cell] + step) & mask);
}

SwayResult sway_formation(const Formation *formation, int32_t frame, int stop_in_middle)
{
    SwayResult result = {*formation, 0};
    if (((frame - 1) & 3) != 0)
        return result;
    const int32_t step = formation->direction == 0 ? 1 : -1;
    for (int cell = 0; cell < COLUMNS; cell++)
        move_cell(&result.formation, cell, step);
    const int32_t first = result.formation.offsets[0];
    if (stop_in_middle && first == 0) {
        result.formation.direction = 0;
        result.settled = 1;
    } else if (first == 0x20) {
        result.formation.direction = 1;
    } else if (first == 0xe0) {
        result.formation.direction = 0;
    }
    return result;
}

/** `c_1E43`: moves the cells whose bitmap bit comes out set, rotating each bitmap byte. */
static void breathe_cells(Formation *f, int first, int last, int32_t step)
{
    for (int cell = first; cell <= last; cell++) {
        const int32_t bits = f->bitmap[cell];
        f->bitmap[cell] = (uint8_t)(((bits >> 1) | (bits << 7)) & 0xff);
        if (bits & 1)
            move_cell(f, cell, step);
    }
}

/** The next value of the breathing counter: 0 up to $1F, then $A0 down to $81, and again. */
static int32_t next_counter(int32_t counter)
{
    const int32_t stepped = (counter & 0x80) ? counter - 1 : counter + 1;
    if (counter == 0x1f)
        return stepped | 0x80;
    if (counter == 0x81)
        return stepped & 0x7f;
    return stepped;
}

Formation breathe_formation(const Formation *formation, int32_t frame)
{
    Formation f = *formation;
    if ((frame & 3) != 0)
        return f;
    const int32_t previous = formation->direction;
    const int32_t counter = next_counter(previous);
    const int32_t row = (counter & 0x18) * 2;
    if ((previous & 7) == 0)
        for (int cell = 0; cell < CELLS; cell++)
            f.bitmap[cell] = (uint8_t)TABLE(BREATHING_BITMAPS, row + cell);
    const int closing = (previous & 0x80) != 0;
    f.direction = (uint8_t)counter;
    /* the left columns move one way; the right columns and the rows the other */
    breathe_cells(&f, 0, 4, closing ? 1 : -1);
    breathe_cells(&f, 5, CELLS - 1, closing ? -1 : 1);
    return f;
}

int32_t formation_offset(const Formation *formation, int32_t cell)
{
    return cell >= 0 && cell < CELLS ? formation->offsets[cell] : 0;
}

int32_t formation_coord(const Formation *formation, int32_t cell)
{
    return cell >= 0 && cell < CELLS ? formation->sprite_coords[cell] : 0;
}
