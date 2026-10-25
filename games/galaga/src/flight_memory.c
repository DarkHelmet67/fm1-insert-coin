/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_memory.h"

#include "rom.h"

/** Address of the formation layout copied into this ROM: (row, column) per object. */
#define FORMATION_LAYOUT 0x0100

int32_t rom_byte(int32_t address)
{
    const int32_t offset = (address & 0xffff) - (int32_t)FLIGHT_ROM_FIRST;
    return offset >= 0 && offset < (int32_t)FLIGHT_ROM_SIZE ? FLIGHT_ROM[offset] : 0xff;
}

int32_t rom_word(int32_t address)
{
    return rom_byte(address) | (rom_byte(address + 1) << 8);
}

HomeCell home_cell(int32_t id)
{
    HomeCell cell;
    cell.row = rom_byte(FORMATION_LAYOUT + (id & 0x7e)) >> 1;
    cell.column = rom_byte(FORMATION_LAYOUT + (id & 0x7e) + 1) >> 1;
    return cell;
}
