/* SPDX-License-Identifier: GPL-3.0-only */
#include "font.h"

#include "screen/draw.h"

/** `c` in uppercase: arcade fonts have no lowercase letters. */
static char upper(char c)
{
    return (c >= 'a' && c <= 'z') ? (char)(c - 'a' + 'A') : c;
}

const Sprite *font_glyph(const PixelFont *font, char c)
{
    const char wanted = upper(c);
    for (uint32_t i = 0; font->chars[i]; i++)
        if (font->chars[i] == wanted)
            return font->glyphs[i];
    return 0;
}

/** Number of characters of a C string. */
static int32_t length(const char *text)
{
    int32_t n = 0;
    while (text[n])
        n++;
    return n;
}

int32_t font_text_width(const PixelFont *font, const char *text)
{
    const int32_t n = length(text);
    return n == 0 ? 0 : n * (font->glyph_width + font->spacing) - font->spacing;
}

void fb_draw_text(Framebuffer *fb, const PixelFont *font, const char *text, int32_t x, int32_t y,
                  uint8_t color)
{
    for (int32_t i = 0; text[i]; i++) {
        const Sprite *glyph = font_glyph(font, text[i]);
        if (glyph)
            fb_draw_sprite(fb, glyph, x + i * (font->glyph_width + font->spacing), y, color);
    }
}

void fb_draw_text_centered(Framebuffer *fb, const PixelFont *font, const char *text, int32_t y,
                           uint8_t color)
{
    /* the screen is never narrower than the text in our games: no negative division here */
    const int32_t x = (fb->width - font_text_width(font, text)) / 2;
    fb_draw_text(fb, font, text, x, y, color);
}
