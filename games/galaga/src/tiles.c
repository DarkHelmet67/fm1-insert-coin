/* SPDX-License-Identifier: GPL-3.0-only */
#include "tiles.h"

#include "art.h"
#include "playfield.h"

/** Side of a character cell. */
#define CELL 8

int32_t sprite_pixel(int32_t code, int32_t x, int32_t y)
{
    if (code < 0 || code >= (int32_t)SPRITE_TILES)
        return 0;
    return (int32_t)(SPRITE_ART[code][y] >> (2 * x)) & 3;
}

int32_t big_sprite_pixel(int32_t code, int32_t x, int32_t y)
{
    int32_t right = x >= SPRITE_SIZE;
    int32_t bottom = y >= SPRITE_SIZE;
    int32_t offset = right ? (bottom ? 1 : 0) : (bottom ? 3 : 2);
    return sprite_pixel(code + offset, x % SPRITE_SIZE, y % SPRITE_SIZE);
}

int32_t char_pixel(int32_t code, int32_t x, int32_t y)
{
    if (code < 0 || code >= (int32_t)CHAR_TILES)
        return 0;
    return (CHAR_ART[code][y] >> (2 * x)) & 3;
}

/** Draws the pixel value `value` at (x, y), unless it is 0 or see-through. */
static void put(Framebuffer *fb, int32_t x, int32_t y, int32_t value, const TileColors *colors)
{
    if (value != 0 && colors->index[value - 1] != SEE_THROUGH_INDEX)
        fb_set(fb, x, y, colors->index[value - 1]);
}

void draw_char(Framebuffer *fb, int32_t code, int32_t column, int32_t row, const TileColors *colors)
{
    for (int32_t y = 0; y < CELL; y++)
        for (int32_t x = 0; x < CELL; x++)
            put(fb, column * CELL + x, row * CELL + y, char_pixel(code, x, y), colors);
}

/** Draws a 16 x 16 sprite at (left, top), mirrored. */
static void draw_sprite_tile(Framebuffer *fb, int32_t code, int32_t left, int32_t top, int flip_x,
                             int flip_y, const TileColors *colors)
{
    for (int32_t y = 0; y < SPRITE_SIZE; y++) {
        int32_t sy = flip_y ? SPRITE_SIZE - 1 - y : y;
        for (int32_t x = 0; x < SPRITE_SIZE; x++) {
            int32_t sx = flip_x ? SPRITE_SIZE - 1 - x : x;
            put(fb, left + x, top + y, sprite_pixel(code, sx, sy), colors);
        }
    }
}

/** Draws a 32 x 32 double size sprite at (left, top), mirrored as a whole. */
static void draw_big_sprite(Framebuffer *fb, int32_t code, int32_t left, int32_t top, int flip_x,
                            int flip_y, const TileColors *colors)
{
    const int32_t size = 2 * SPRITE_SIZE;
    for (int32_t y = 0; y < size; y++) {
        int32_t sy = flip_y ? size - 1 - y : y;
        for (int32_t x = 0; x < size; x++) {
            int32_t sx = flip_x ? size - 1 - x : x;
            put(fb, left + x, top + y, big_sprite_pixel(code, sx, sy), colors);
        }
    }
}

void draw_object(Framebuffer *fb, const GameObject *object)
{
    const TileColors colors = sprite_colors(object->color);
    int32_t left = object->x - SPRITE_OFFSET_X;
    int32_t top = object->y - SPRITE_OFFSET_Y;
    if (object->big) {
        draw_big_sprite(fb, object->code & 0x7c, left, top, object->flip_x, object->flip_y,
                        &colors);
    } else if (object->wide) {
        /* A double width sprite puts code + 2 on the left and the code itself on the right. */
        draw_sprite_tile(fb, object->code + 2, left, top, object->flip_x, object->flip_y, &colors);
        draw_sprite_tile(fb, object->code, left + SPRITE_SIZE, top, object->flip_x, object->flip_y,
                         &colors);
    } else {
        draw_sprite_tile(fb, object->code, left, top, object->flip_x, object->flip_y, &colors);
    }
}

void draw_objects(Framebuffer *fb, const Objects *objects)
{
    for (int32_t i = 0; i < OBJECT_COUNT; i++)
        if (objects->list[i].x != 0)
            draw_object(fb, &objects->list[i]);
}
