/* SPDX-License-Identifier: GPL-3.0-only */
#include "bitmap.h"

/** The bit of column x in a row. */
static uint32_t column_bit(int32_t x)
{
    return 1u << (31 - x);
}

/** Whether the placed bitmap has a lit pixel at the screen position (sx, sy). */
static int placed_lit(const Placed *placed, int32_t sx, int32_t sy)
{
    return bitmap_lit(placed->bitmap, sx - placed->x, sy - placed->y);
}

/** The larger of two numbers. */
static int32_t max_i32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

/** The smaller of two numbers. */
static int32_t min_i32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

Bitmap bitmap_from_sprite(const Sprite *sprite)
{
    Bitmap bitmap = {0, 0, {0}};
    bitmap.width = (uint8_t)min_i32(sprite->width, BITMAP_MAX_WIDTH);
    bitmap.height = (uint8_t)min_i32(sprite->height, BITMAP_MAX_ROWS);
    for (int32_t y = 0; y < bitmap.height; y++)
        for (int32_t x = 0; x < bitmap.width; x++)
            if (sprite_lit(sprite, x, y))
                bitmap.rows[y] |= column_bit(x);
    return bitmap;
}

int bitmap_lit(const Bitmap *bitmap, int32_t x, int32_t y)
{
    if (x < 0 || y < 0 || x >= bitmap->width || y >= bitmap->height)
        return 0;
    return (bitmap->rows[y] & column_bit(x)) != 0u;
}

uint32_t bitmap_count(const Bitmap *bitmap)
{
    uint32_t count = 0;
    for (int32_t y = 0; y < bitmap->height; y++)
        for (int32_t x = 0; x < bitmap->width; x++)
            count += (uint32_t)bitmap_lit(bitmap, x, y);
    return count;
}

int placed_overlap(const Placed *a, const Placed *b)
{
    /* first the shared rectangle, then its pixels one by one */
    const int32_t x0 = max_i32(a->x, b->x);
    const int32_t y0 = max_i32(a->y, b->y);
    const int32_t x1 = min_i32(a->x + a->bitmap->width, b->x + b->bitmap->width);
    const int32_t y1 = min_i32(a->y + a->bitmap->height, b->y + b->bitmap->height);
    for (int32_t y = y0; y < y1; y++)
        for (int32_t x = x0; x < x1; x++)
            if (placed_lit(a, x, y) && placed_lit(b, x, y))
                return 1;
    return 0;
}

Bitmap bitmap_erase(const Placed *target, const Placed *brush)
{
    Bitmap erased = *target->bitmap;
    for (int32_t y = 0; y < erased.height; y++)
        for (int32_t x = 0; x < erased.width; x++)
            if (placed_lit(brush, target->x + x, target->y + y))
                erased.rows[y] &= ~column_bit(x);
    return erased;
}
