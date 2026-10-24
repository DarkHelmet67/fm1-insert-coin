/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * A fixed-size text buffer to build the canonical state string of a frame (tools/traces/FORMAT.md)
 * without the C library: the same code runs on the computer and, if needed, on the FM-1.
 */
#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

/** Room for the longest state string (55 invaders and four shields take about 2 KB). */
#define TEXT_CAPACITY 4096u

/** Text being built; `overflow` says that something did not fit and was cut. */
typedef struct {
    char data[TEXT_CAPACITY];
    uint32_t length;
    int overflow;
} Text;

/** Empties the text. */
void text_clear(Text *text);

/** Appends a string (NUL-terminated). */
void text_add(Text *text, const char *s);

/** Appends one character. */
void text_add_char(Text *text, char c);

/** Appends a whole number in decimal, with a minus sign when negative. */
void text_add_int(Text *text, int32_t value);

/** Appends the low `digits` hex digits of a number, lowercase, leading zeros kept. */
void text_add_hex(Text *text, uint32_t value, uint32_t digits);

/** The text as a NUL-terminated string (valid until the next change). */
const char *text_cstr(Text *text);

/** FNV-1a, 32 bits, of the text's bytes: the hash written in the traces. */
uint32_t text_fnv1a(const Text *text);

#endif
