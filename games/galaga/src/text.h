/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Texts in the ROM font (text.ts, draw-texts.ts): the screen is a grid of 28 x 36 cells of 8 x 8
 * pixels, and the program writes its strings into it.
 */
#ifndef GALAGA_TEXT_H
#define GALAGA_TEXT_H

#include <stdint.h>

#include "cartridge/private.h"
#include "messages.h"
#include "screen/framebuffer.h"

/**
 * The character code that shows `c`. Digits are codes 0-9 and letters follow from 0x0A: the
 * original program writes its strings in ASCII and subtracts 0x30 or 0x37. Unknown characters
 * are blank; 0xA9 is the copyright sign.
 */
GAME_PRIVATE int32_t char_code(char c);

/**
 * Writes `text` from the cell at `column`, `row`, in character palette `palette` (the colour
 * byte the original program puts before each of its strings).
 */
GAME_PRIVATE void draw_cell_text(Framebuffer *fb, const char *text, int32_t column, int32_t row,
                                 int32_t palette);

/** The texts of the main program. */
GAME_PRIVATE void draw_messages(Framebuffer *fb, const Messages *messages);

#endif
