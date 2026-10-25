/* SPDX-License-Identifier: GPL-3.0-only */
#include "attract.h"

/** The texts of the info screen, one a second (strings $0E-$12 of `d_cstring_tbl`). */
static const struct {
    const char *text;
    int8_t column;
    int8_t row;
} INFO_TEXTS[] = {
    {"GALAGA", 11, 4},     {"-- SCORE --", 8, 7}, {"50    100", 12, 10},
    {"80    160", 12, 12}, {"", 12, 13},
};

/** Number of texts of the info screen. */
#define INFO_TEXT_COUNT 5

/** `s_1525`: the copyright line (the styled "NAMCO" under it is left out); \xa9 is the (C). */
#define COPYRIGHT_TEXT "\xa9 1981 NAMCO LTD."
#define COPYRIGHT_COLUMN 6
#define COPYRIGHT_ROW 29
#define COPYRIGHT_PALETTE 3

/**
 * `d_attrmode_sptiles`: the aliens of the score table, from the 3rd text on: object, colour and
 * code, x and y / 2. The upright sprite of each alien is its code + 6.
 */
static const struct {
    uint8_t object;
    uint8_t look;
    uint8_t x;
    uint8_t y;
} INFO_SPRITES[3] = {{0x08, 0x1b, 0x44, 0x3a}, {0x0a, 0x12, 0x44, 0x42}, {0x0c, 0x08, 0x7c, 0x50}};

/** `case_1940`, `case_1948`: a clear screen, the first text due in one second. */
static void start_info(Attract *attract)
{
    attract->timers = set_timer(&attract->timers, 2, 2);
    attract->step = ATTRACT_INFO;
    attract->shown = 0;
    attract->messages = no_messages();
    attract->objects = empty_objects();
}

void new_attract(Attract *attract)
{
    attract->frame = 0;
    attract->stars.phase = 0;
    attract->stars.speed = STARS_SLOW;
    attract->timers = no_timers();
    start_info(attract);
}

/** `c_sprite_tiles_displ`: an alien of the score table appears, upright. */
static void show_sprite(Objects *objects, int32_t index)
{
    if (index < 0 || index >= 3)
        return;
    GameObject *object = object_ref(objects, INFO_SPRITES[index].object);
    int32_t look = INFO_SPRITES[index].look;
    object->disposition = DISPOSITION_HOME;
    object->code = (uint8_t)((look & 0x78) + 6);
    object->color = (uint8_t)((look & 0x07) | ((look & 0x80) != 0 ? 8 : 0));
    object->x = INFO_SPRITES[index].x;
    object->y = (uint16_t)(INFO_SPRITES[index].y * 2);
}

/** `case_1984`: every second the next text, and from the third one an alien. */
static void info(Attract *attract)
{
    if (attract->timers.t[2] != 0)
        return;
    attract->timers = set_timer(&attract->timers, 2, 2);
    if (attract->shown == INFO_TEXT_COUNT) {
        /* The copyright stays about four seconds (`case_18AC`). */
        const Message copyright =
            message_at(COPYRIGHT_TEXT, COPYRIGHT_COLUMN, COPYRIGHT_ROW, COPYRIGHT_PALETTE);
        attract->timers = set_timer(&attract->timers, 2, 9);
        attract->step = ATTRACT_COPYRIGHT;
        attract->messages = messages_show(&attract->messages, &copyright);
        return;
    }
    int32_t index = attract->shown++;
    if (INFO_TEXTS[index].text[0] != '\0') {
        const Message text =
            message_at(INFO_TEXTS[index].text, INFO_TEXTS[index].column, INFO_TEXTS[index].row, 0);
        attract->messages = messages_show(&attract->messages, &text);
    }
    if (attract->shown >= 3)
        show_sprite(&attract->objects, attract->shown - 3);
}

void update_attract(Attract *attract)
{
    attract->frame++;
    attract->stars.phase = advance_stars(attract->stars.phase, attract->stars.speed);
    attract->timers = tick_timers(&attract->timers, attract->frame);
    if (attract->step == ATTRACT_INFO)
        info(attract);
    else if (attract->timers.t[2] <= 1)
        start_info(attract);
}
