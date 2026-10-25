/* SPDX-License-Identifier: GPL-3.0-only */
#include "arcade_font.h"

/* clang-format off */
SPRITE_DEFINE(GLYPH_A, 5, ".XXX.", "X...X", "X...X", "XXXXX", "X...X", "X...X", "X...X");
SPRITE_DEFINE(GLYPH_B, 5, "XXXX.", "X...X", "X...X", "XXXX.", "X...X", "X...X", "XXXX.");
SPRITE_DEFINE(GLYPH_C, 5, ".XXX.", "X...X", "X....", "X....", "X....", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_D, 5, "XXXX.", "X...X", "X...X", "X...X", "X...X", "X...X", "XXXX.");
SPRITE_DEFINE(GLYPH_E, 5, "XXXXX", "X....", "X....", "XXXX.", "X....", "X....", "XXXXX");
SPRITE_DEFINE(GLYPH_F, 5, "XXXXX", "X....", "X....", "XXXX.", "X....", "X....", "X....");
SPRITE_DEFINE(GLYPH_G, 5, ".XXXX", "X....", "X....", "X..XX", "X...X", "X...X", ".XXXX");
SPRITE_DEFINE(GLYPH_H, 5, "X...X", "X...X", "X...X", "XXXXX", "X...X", "X...X", "X...X");
SPRITE_DEFINE(GLYPH_I, 5, ".XXX.", "..X..", "..X..", "..X..", "..X..", "..X..", ".XXX.");
SPRITE_DEFINE(GLYPH_J, 5, "....X", "....X", "....X", "....X", "....X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_K, 5, "X...X", "X..X.", "X.X..", "XX...", "X.X..", "X..X.", "X...X");
SPRITE_DEFINE(GLYPH_L, 5, "X....", "X....", "X....", "X....", "X....", "X....", "XXXXX");
SPRITE_DEFINE(GLYPH_M, 5, "X...X", "XX.XX", "X.X.X", "X.X.X", "X...X", "X...X", "X...X");
SPRITE_DEFINE(GLYPH_N, 5, "X...X", "X...X", "XX..X", "X.X.X", "X..XX", "X...X", "X...X");
SPRITE_DEFINE(GLYPH_O, 5, ".XXX.", "X...X", "X...X", "X...X", "X...X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_P, 5, "XXXX.", "X...X", "X...X", "XXXX.", "X....", "X....", "X....");
SPRITE_DEFINE(GLYPH_Q, 5, ".XXX.", "X...X", "X...X", "X...X", "X.X.X", "X..X.", ".XX.X");
SPRITE_DEFINE(GLYPH_R, 5, "XXXX.", "X...X", "X...X", "XXXX.", "X.X..", "X..X.", "X...X");
SPRITE_DEFINE(GLYPH_S, 5, ".XXXX", "X....", "X....", ".XXX.", "....X", "....X", "XXXX.");
SPRITE_DEFINE(GLYPH_T, 5, "XXXXX", "..X..", "..X..", "..X..", "..X..", "..X..", "..X..");
SPRITE_DEFINE(GLYPH_U, 5, "X...X", "X...X", "X...X", "X...X", "X...X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_V, 5, "X...X", "X...X", "X...X", "X...X", "X...X", ".X.X.", "..X..");
SPRITE_DEFINE(GLYPH_W, 5, "X...X", "X...X", "X...X", "X.X.X", "X.X.X", "XX.XX", "X...X");
SPRITE_DEFINE(GLYPH_X, 5, "X...X", "X...X", ".X.X.", "..X..", ".X.X.", "X...X", "X...X");
SPRITE_DEFINE(GLYPH_Y, 5, "X...X", "X...X", ".X.X.", "..X..", "..X..", "..X..", "..X..");
SPRITE_DEFINE(GLYPH_Z, 5, "XXXXX", "....X", "...X.", "..X..", ".X...", "X....", "XXXXX");
SPRITE_DEFINE(GLYPH_0, 5, ".XXX.", "X...X", "X..XX", "X.X.X", "XX..X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_1, 5, "..X..", ".XX..", "..X..", "..X..", "..X..", "..X..", ".XXX.");
SPRITE_DEFINE(GLYPH_2, 5, ".XXX.", "X...X", "....X", "...X.", "..X..", ".X...", "XXXXX");
SPRITE_DEFINE(GLYPH_3, 5, "XXXXX", "...X.", "..X..", "...X.", "....X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_4, 5, "...X.", "..XX.", ".X.X.", "X..X.", "XXXXX", "...X.", "...X.");
SPRITE_DEFINE(GLYPH_5, 5, "XXXXX", "X....", "XXXX.", "....X", "....X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_6, 5, "..XX.", ".X...", "X....", "XXXX.", "X...X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_7, 5, "XXXXX", "....X", "...X.", "..X..", ".X...", ".X...", ".X...");
SPRITE_DEFINE(GLYPH_8, 5, ".XXX.", "X...X", "X...X", ".XXX.", "X...X", "X...X", ".XXX.");
SPRITE_DEFINE(GLYPH_9, 5, ".XXX.", "X...X", "X...X", ".XXXX", "....X", "...X.", ".XX..");
SPRITE_DEFINE(GLYPH_SPACE, 5, ".....", ".....", ".....", ".....", ".....", ".....", ".....");
SPRITE_DEFINE(GLYPH_DASH, 5, ".....", ".....", ".....", "XXXXX", ".....", ".....", ".....");
SPRITE_DEFINE(GLYPH_EQUALS, 5, ".....", ".....", "XXXXX", ".....", "XXXXX", ".....", ".....");
SPRITE_DEFINE(GLYPH_STAR, 5, ".....", "X.X.X", ".XXX.", "XXXXX", ".XXX.", "X.X.X", ".....");
SPRITE_DEFINE(GLYPH_QUESTION, 5, ".XXX.", "X...X", "....X", "...X.", "..X..", ".....", "..X..");
SPRITE_DEFINE(GLYPH_LESS, 5, "...X.", "..X..", ".X...", "X....", ".X...", "..X..", "...X.");
SPRITE_DEFINE(GLYPH_GREATER, 5, ".X...", "..X..", "...X.", "....X", "...X.", "..X..", ".X...");
SPRITE_DEFINE(GLYPH_DOT, 5, ".....", ".....", ".....", ".....", ".....", ".....", "..X..");
/* clang-format on */

/** The glyphs, in the order of the characters below. */
static const Sprite *const GLYPHS[] = {
    &GLYPH_A,       &GLYPH_B,    &GLYPH_C,      &GLYPH_D,    &GLYPH_E,        &GLYPH_F,
    &GLYPH_G,       &GLYPH_H,    &GLYPH_I,      &GLYPH_J,    &GLYPH_K,        &GLYPH_L,
    &GLYPH_M,       &GLYPH_N,    &GLYPH_O,      &GLYPH_P,    &GLYPH_Q,        &GLYPH_R,
    &GLYPH_S,       &GLYPH_T,    &GLYPH_U,      &GLYPH_V,    &GLYPH_W,        &GLYPH_X,
    &GLYPH_Y,       &GLYPH_Z,    &GLYPH_0,      &GLYPH_1,    &GLYPH_2,        &GLYPH_3,
    &GLYPH_4,       &GLYPH_5,    &GLYPH_6,      &GLYPH_7,    &GLYPH_8,        &GLYPH_9,
    &GLYPH_SPACE,   &GLYPH_DASH, &GLYPH_EQUALS, &GLYPH_STAR, &GLYPH_QUESTION, &GLYPH_LESS,
    &GLYPH_GREATER, &GLYPH_DOT,
};

const PixelFont ARCADE_FONT = {5, 7, 3, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -=*?<>.", GLYPHS};
