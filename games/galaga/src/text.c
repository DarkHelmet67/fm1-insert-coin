/* SPDX-License-Identifier: GPL-3.0-only */
#include "text.h"

#include "palette.h"
#include "tiles.h"

/** Character codes of the symbols in the ROM font. */
#define CHAR_BLANK 0x24

int32_t char_code(char c)
{
    if (c >= '0' && c <= '9')
        return c - 0x30;
    if (c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 'A');
    if (c >= 'A' && c <= 'Z')
        return c - 0x37;
    switch ((unsigned char)c) {
    case '-':
        return 0x26;
    case '%':
        return 0x29;
    case '.':
        return 0x2a;
    case '!':
        return 0x2c;
    case 0xa9:
        return 0x2e;
    default:
        return CHAR_BLANK;
    }
}

void draw_cell_text(Framebuffer *fb, const char *text, int32_t column, int32_t row, int32_t palette)
{
    const TileColors colors = char_colors(palette);
    for (int32_t i = 0; text[i] != '\0'; i++)
        draw_char(fb, char_code(text[i]), column + i, row, &colors);
}

void draw_messages(Framebuffer *fb, const Messages *messages)
{
    for (uint32_t i = 0; i < messages->count; i++) {
        const Message *m = &messages->list[i];
        draw_cell_text(fb, m->text, m->column, m->row, m->palette);
    }
}
