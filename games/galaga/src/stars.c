/* SPDX-License-Identifier: GPL-3.0-only */
#include "stars.h"

#include "playfield.h"
#include "rom.h"

/** Steps before the register repeats. */
#define LFSR_PERIOD 65535

/** Raster pixels per line, and lines of the raster that reach the screen. */
#define LINE 256
#define VISIBLE_LINES 224

/** Register steps before and after the visible lines of a frame, when nothing scrolls. */
#define PRE_VISIBLE (22 * LINE)
#define POST_VISIBLE (10 * LINE)

/** The starfield raster starts 16 pixels into the screen's long side. */
#define RASTER_OFFSET 16

/** Extra register steps per frame for each of the 8 scroll speeds. */
static const int32_t SPEED_EXTRA_STEPS[8] = {0, 1, 2, 3, -4, -3, -2, -1};

/** `value` modulo the register period, always positive. */
static int32_t wrap(int32_t value)
{
    return ((value % LFSR_PERIOD) + LFSR_PERIOD) % LFSR_PERIOD;
}

/** Register steps run before the first visible pixel of a frame at `speed`. */
static int32_t pre_visible_steps(int32_t speed)
{
    return PRE_VISIBLE + SPEED_EXTRA_STEPS[speed & 7];
}

int32_t advance_stars(int32_t phase, int32_t speed)
{
    return wrap(phase + pre_visible_steps(speed) + VISIBLE_LINES * LINE + POST_VISIBLE);
}

void star_sets(int32_t frame, int32_t sets[2])
{
    int32_t bits = frame & 0x1c;
    int32_t select = ((bits >> 1) ^ bits) & 0x18;
    sets[0] = (select >> 3) & 1;
    sets[1] = ((select >> 4) & 1) | 2;
}

int32_t visible_stars(int32_t phase, int32_t speed, int32_t frame, Star *out)
{
    int32_t start = phase + pre_visible_steps(speed);
    int32_t sets[2];
    int32_t count = 0;
    star_sets(frame, sets);
    for (uint32_t i = 0; i < STAR_COUNT; i++) {
        const StarSeed *star = &STAR_SEEDS[i];
        int32_t pixel = wrap(star->step - start);
        int32_t line = pixel / LINE;
        int32_t y = RASTER_OFFSET + pixel % LINE;
        if (line >= VISIBLE_LINES || (star->set != sets[0] && star->set != sets[1]) ||
            y >= SCREEN_HEIGHT)
            continue;
        out[count].x = (uint8_t)(SCREEN_WIDTH - 1 - line);
        out[count].y = (uint16_t)y;
        out[count].color = star->color;
        count++;
    }
    return count;
}
