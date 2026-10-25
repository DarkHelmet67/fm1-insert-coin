/* SPDX-License-Identifier: GPL-3.0-only */
#include "text.h"

void text_clear(Text *text)
{
    text->length = 0;
    text->overflow = 0;
}

void text_add_char(Text *text, char c)
{
    if (text->length + 1u >= TEXT_CAPACITY) { /* one byte stays for the NUL */
        text->overflow = 1;
        return;
    }
    text->data[text->length++] = c;
}

void text_add(Text *text, const char *s)
{
    while (*s)
        text_add_char(text, *s++);
}

void text_add_int(Text *text, int32_t value)
{
    char digits[11];
    uint32_t count = 0;
    /* the magnitude as unsigned: -2147483648 has no positive int32 */
    uint32_t magnitude = value < 0 ? 0u - (uint32_t)value : (uint32_t)value;
    if (value < 0)
        text_add_char(text, '-');
    do {
        digits[count++] = (char)('0' + magnitude % 10u);
        magnitude /= 10u;
    } while (magnitude);
    while (count)
        text_add_char(text, digits[--count]);
}

void text_add_hex(Text *text, uint32_t value, uint32_t digits)
{
    static const char HEX[] = "0123456789abcdef";
    for (uint32_t i = digits; i > 0; i--)
        text_add_char(text, HEX[(value >> ((i - 1u) * 4u)) & 15u]);
}

const char *text_cstr(Text *text)
{
    text->data[text->length] = '\0';
    return text->data;
}

uint32_t text_fnv1a(const Text *text)
{
    uint32_t hash = 0x811C9DC5u;
    for (uint32_t i = 0; i < text->length; i++) {
        hash ^= (uint8_t)text->data[i];
        hash *= 0x01000193u;
    }
    return hash;
}
