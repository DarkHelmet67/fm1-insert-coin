/* SPDX-License-Identifier: GPL-3.0-only */
#include "aliens.h"

/** Kind of invader in each of the five rows, from top to bottom. */
static const AlienKind ROW_KINDS[FORMATION_ROWS] = {
    ALIEN_SQUID, ALIEN_CRAB, ALIEN_CRAB, ALIEN_OCTOPUS, ALIEN_OCTOPUS,
};

/**
 * Height of the bottom row at the start of rounds 2 to 9, from the original's table at 0x1DA3
 * (0x60, 0x50, 0x48, 0x48, 0x48, 0x40, 0x40, 0x40) converted to our coordinates.
 */
static const int16_t ROUND_START_BOTTOM_Y[8] = {152, 168, 176, 176, 176, 184, 184, 184};

/** Points for each kind of invader. */
static const int32_t POINTS[ALIEN_KIND_COUNT] = {30, 20, 10};

/** Width of the widest invader: narrower ones are centred in that space. */
#define WIDEST_ALIEN 12

int32_t alien_width(const Alien *alien)
{
    return alien_sprite((AlienKind)alien->kind, 0)->width;
}

int32_t alien_points(const Alien *alien)
{
    return POINTS[alien->kind];
}

int32_t start_bottom_y(uint32_t round)
{
    if (round <= 1u)
        return FIRST_ROUND_BOTTOM_Y;
    return ROUND_START_BOTTOM_Y[(round - 2u) % 8u];
}

Alien formation_alien(uint32_t index, uint32_t round)
{
    const uint32_t row_from_bottom = index / FORMATION_COLUMNS;
    const uint32_t column = index % FORMATION_COLUMNS;
    const AlienKind kind = ROW_KINDS[FORMATION_ROWS - 1u - row_from_bottom];
    const int32_t width = alien_sprite(kind, 0)->width;
    Alien alien;
    alien.kind = (uint8_t)kind;
    alien.row = (uint8_t)(FORMATION_ROWS - row_from_bottom);
    alien.column = (uint8_t)(column + 1u);
    alien.frame = 0;
    alien.x = (int16_t)(FORMATION_LEFT + (int32_t)column * CELL_SIZE + (WIDEST_ALIEN - width) / 2);
    alien.y = (int16_t)(start_bottom_y(round) - (int32_t)row_from_bottom * CELL_SIZE);
    return alien;
}
