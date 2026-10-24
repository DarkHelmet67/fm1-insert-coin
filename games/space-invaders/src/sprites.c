/* SPDX-License-Identifier: GPL-3.0-only */
#include "sprites.h"

/* clang-format off */
SPRITE_DEFINE(SQUID_0, 8,
    "...XX...",
    "..XXXX..",
    ".XXXXXX.",
    "XX.XX.XX",
    "XXXXXXXX",
    "..X..X..",
    ".X.XX.X.",
    "X.X..X.X");
SPRITE_DEFINE(SQUID_1, 8,
    "...XX...",
    "..XXXX..",
    ".XXXXXX.",
    "XX.XX.XX",
    "XXXXXXXX",
    ".X.XX.X.",
    "X......X",
    ".X....X.");
SPRITE_DEFINE(CRAB_0, 11,
    "..X.....X..",
    "...X...X...",
    "..XXXXXXX..",
    ".XX.XXX.XX.",
    "XXXXXXXXXXX",
    "X.XXXXXXX.X",
    "X.X.....X.X",
    "...XX.XX...");
SPRITE_DEFINE(CRAB_1, 11,
    "..X.....X..",
    "X..X...X..X",
    "X.XXXXXXX.X",
    "XXX.XXX.XXX",
    "XXXXXXXXXXX",
    ".XXXXXXXXX.",
    "..X.....X..",
    ".X.......X.");
SPRITE_DEFINE(OCTOPUS_0, 12,
    "....XXXX....",
    ".XXXXXXXXXX.",
    "XXXXXXXXXXXX",
    "XXX..XX..XXX",
    "XXXXXXXXXXXX",
    "...XX..XX...",
    "..XX.XX.XX..",
    "XX........XX");
SPRITE_DEFINE(OCTOPUS_1, 12,
    "....XXXX....",
    ".XXXXXXXXXX.",
    "XXXXXXXXXXXX",
    "XXX..XX..XXX",
    "XXXXXXXXXXXX",
    "..XXX..XXX..",
    ".XX..XX..XX.",
    "..XX....XX..");

SPRITE_DEFINE(CANNON_SPRITE, 13,
    "......X......",
    ".....XXX.....",
    ".....XXX.....",
    ".XXXXXXXXXXX.",
    "XXXXXXXXXXXXX",
    "XXXXXXXXXXXXX",
    "XXXXXXXXXXXXX",
    "XXXXXXXXXXXXX");

SPRITE_DEFINE(UFO_SPRITE, 16,
    ".....XXXXXX.....",
    "...XXXXXXXXXX...",
    "..XXXXXXXXXXXX..",
    ".XX.XX.XX.XX.XX.",
    "XXXXXXXXXXXXXXXX",
    "..XXX..XX..XXX..",
    "...X........X...");

SPRITE_DEFINE(SHOT_SPRITE, 1, "X", "X", "X", "X");

SPRITE_DEFINE(EXPLOSION_SPRITE, 13,
    "....X...X....",
    ".X...X.X...X.",
    "..X.......X..",
    "...X.....X...",
    "XX.........XX",
    "...X.....X...",
    "..X..X.X..X..",
    ".X..X...X..X.");

SPRITE_DEFINE(ROLLING_0, 3, ".X.", ".X.", "XX.", ".X.", ".X.", ".XX", ".X.", ".X.");
SPRITE_DEFINE(ROLLING_1, 3, ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.");
SPRITE_DEFINE(ROLLING_2, 3, ".X.", ".X.", ".XX", ".X.", ".X.", "XX.", ".X.", ".X.");
SPRITE_DEFINE(ROLLING_3, 3, ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.");
SPRITE_DEFINE(PLUNGER_0, 3, "XXX", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", ".X.");
SPRITE_DEFINE(PLUNGER_1, 3, ".X.", ".X.", "XXX", ".X.", ".X.", ".X.", ".X.", ".X.");
SPRITE_DEFINE(PLUNGER_2, 3, ".X.", ".X.", ".X.", ".X.", "XXX", ".X.", ".X.", ".X.");
SPRITE_DEFINE(PLUNGER_3, 3, ".X.", ".X.", ".X.", ".X.", ".X.", ".X.", "XXX", ".X.");
SPRITE_DEFINE(SQUIGGLY_0, 3, "X..", ".X.", "..X", ".X.", "X..", ".X.", "..X", ".X.");
SPRITE_DEFINE(SQUIGGLY_1, 3, ".X.", "..X", ".X.", "X..", ".X.", "..X", ".X.", "X..");
SPRITE_DEFINE(SQUIGGLY_2, 3, "..X", ".X.", "X..", ".X.", "..X", ".X.", "X..", ".X.");
SPRITE_DEFINE(SQUIGGLY_3, 3, ".X.", "X..", ".X.", "..X", ".X.", "X..", ".X.", "..X");

SPRITE_DEFINE(SHOT_EXPLOSION_SPRITE, 8,
    "X...X..X",
    "..X...X.",
    ".XXXXXX.",
    "XXXXXXXX",
    "XXXXXXXX",
    ".XXXXXX.",
    "..X..X..",
    "X..X...X");

SPRITE_DEFINE(BOMB_EXPLOSION_SPRITE, 6,
    ".X..X.",
    "X.XX..",
    ".XXXX.",
    "XXXXXX",
    ".XXXX.",
    "X.XX.X",
    "..X..X",
    "X...X.");

SPRITE_DEFINE(CANNON_EXPLOSION_0, 14,
    "......X.......",
    "..........X...",
    "...X..X..X....",
    ".....XX.......",
    "..X.XXXX...X..",
    "X..XXXXXX.....",
    ".XXXXXXXXX..X.",
    "XXXXXXXXXXX.XX");
SPRITE_DEFINE(CANNON_EXPLOSION_1, 14,
    "....X....X....",
    "X......X....X.",
    "..X..XX...X...",
    ".X..XXXX......",
    "...XXXXX.X..X.",
    "X.XXXXXXX..X..",
    "..XXXXXXXXX..X",
    ".XXXXXXXXXXXX.");

SPRITE_DEFINE(SHIELD_SPRITE, 22,
    "....XXXXXXXXXXXXXX....",
    "...XXXXXXXXXXXXXXXX...",
    "..XXXXXXXXXXXXXXXXXX..",
    ".XXXXXXXXXXXXXXXXXXXX.",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXXXXXXXXXXXXXXXXX",
    "XXXXXXX........XXXXXXX",
    "XXXXXX..........XXXXXX",
    "XXXXX............XXXXX",
    "XXXXX............XXXXX");
/* clang-format on */

/** Both animation frames of each invader kind. */
static const Sprite *const ALIENS[ALIEN_KIND_COUNT][2] = {
    {&SQUID_0, &SQUID_1},
    {&CRAB_0, &CRAB_1},
    {&OCTOPUS_0, &OCTOPUS_1},
};

/** The four animation frames of each bomb kind. */
static const Sprite *const BOMBS[BOMB_KIND_COUNT][4] = {
    {&ROLLING_0, &ROLLING_1, &ROLLING_2, &ROLLING_3},
    {&PLUNGER_0, &PLUNGER_1, &PLUNGER_2, &PLUNGER_3},
    {&SQUIGGLY_0, &SQUIGGLY_1, &SQUIGGLY_2, &SQUIGGLY_3},
};

const Sprite *alien_sprite(AlienKind kind, uint32_t frame)
{
    return ALIENS[kind][frame % 2u];
}

const Sprite *bomb_sprite(BombKind kind, uint32_t steps)
{
    return BOMBS[kind][steps % 4u];
}

const Sprite *cannon_explosion_sprite(uint32_t frame)
{
    return frame % 2u ? &CANNON_EXPLOSION_1 : &CANNON_EXPLOSION_0;
}
