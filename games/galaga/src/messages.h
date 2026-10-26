/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The texts the main program writes in the middle of the screen ("PLAYER 1", "STAGE 1",
 * "READY"...) (messages.ts). The original writes them into the character memory and erases them
 * by writing a row of spaces over them; the port keeps a list of texts on screen.
 */
#ifndef GALAGA_MESSAGES_H
#define GALAGA_MESSAGES_H

#include <stdint.h>

#include "cartridge/private.h"

/** The longest text, "SPECIAL BONUS 10000 PTS", and its end. */
#define MESSAGE_CHARS 24u

/** The most texts on screen at once: the results of the game are seven. */
#define MESSAGE_SLOTS 12u

/** A text written at a character cell, in a character palette. */
typedef struct {
    char text[MESSAGE_CHARS];
    int8_t column;
    int8_t row;
    int8_t palette;
} Message;

/** The texts on screen, in the order they were written. */
typedef struct {
    Message list[MESSAGE_SLOTS];
    uint8_t count;
} Messages;

/** The texts of the main program with their cells (`d_cstring_tbl`). */
typedef enum {
    MESSAGE_PLAYER_ONE,
    MESSAGE_PLAYER_ONE_ABOVE, /**< above "STAGE 1", written by `plyr_respawn_plyrup` */
    MESSAGE_READY,
    MESSAGE_GAME_OVER,
    MESSAGE_CHALLENGING_STAGE,
    MESSAGE_FIGHTER_CAPTURED,
    MESSAGE_NUMBER_OF_HITS,
    MESSAGE_BONUS,
    MESSAGE_PERFECT,
    MESSAGE_SPECIAL_BONUS,
    MESSAGE_RESULTS,
    MESSAGE_SHOTS_FIRED,
    MESSAGE_RESULT_HITS,
    MESSAGE_HIT_MISS_RATIO,
    MESSAGE_COUNT
} MessageName;

/** No texts. */
GAME_PRIVATE Messages no_messages(void);

/** A text of the main program. */
GAME_PRIVATE Message message_of(MessageName name);

/** A text of its own at a cell. */
GAME_PRIVATE Message message_at(const char *text, int32_t column, int32_t row, int32_t palette);

/** "STAGE n": the number is written after the text. */
GAME_PRIVATE Message stage_message(int32_t stage);

/** The texts with `message` added, replacing one at the same cell. */
GAME_PRIVATE Messages messages_show(const Messages *messages, const Message *message);

/** The texts with row `row` erased (the 27 spaces of string $0B). */
GAME_PRIVATE Messages messages_erase_row(const Messages *messages, int32_t row);

/** Whether a text starts at the cell. */
GAME_PRIVATE int messages_shown_at(const Messages *messages, int32_t column, int32_t row);

/** Writes a number in decimal into `out` (at least 12 chars); returns `out`. */
GAME_PRIVATE char *decimal(int32_t value, char *out);

#endif
