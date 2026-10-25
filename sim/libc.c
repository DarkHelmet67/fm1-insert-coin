/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The few C library functions the games need in the browser simulator: the compiler calls them to
 * copy and clear structs. WebAssembly built with -nostdlib has no C library, as the FM-1 has
 * only Dinghy's (platform/firmware/src/libc.c).
 */
#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
int memcmp(const void *a, const void *b, size_t n);

void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--)
        *d++ = *s++;
    return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s)
        return memcpy(dest, src, n);
    while (n--)
        d[n] = s[n];
    return dest;
}

void *memset(void *dest, int c, size_t n)
{
    unsigned char *d = dest;
    while (n--)
        *d++ = (unsigned char)c;
    return dest;
}

int memcmp(const void *a, const void *b, size_t n)
{
    const unsigned char *x = a, *y = b;
    for (; n--; x++, y++)
        if (*x != *y)
            return *x - *y;
    return 0;
}
