/* SPDX-License-Identifier: GPL-3.0-only */
#include "messages.h"

/** The texts of the main program, in the order of MessageName. */
static const Message MESSAGES[MESSAGE_COUNT] = {
    {"PLAYER 1", 11, 18, 0},
    {"PLAYER 1", 10, 16, 0},
    {"READY", 10, 18, 0},
    {"GAME OVER", 10, 18, 0},
    {"CHALLENGING STAGE", 5, 18, 0},
    {"FIGHTER CAPTURED", 6, 19, 4},
    {"NUMBER OF HITS", 5, 18, 0},
    {"BONUS", 8, 21, 0},
    {"PERFECT !", 10, 15, 4},
    {"SPECIAL BONUS 10000 PTS", 2, 21, 5},
    {"-RESULTS-", 9, 17, 4},
    {"SHOTS FIRED", 4, 20, 5},
    {"NUMBER OF HITS", 4, 23, 5},
    {"HIT-MISS RATIO", 4, 26, 3},
};

Messages no_messages(void)
{
    Messages messages;
    messages.count = 0;
    return messages;
}

Message message_of(MessageName name)
{
    return MESSAGES[name];
}

Message message_at(const char *text, int32_t column, int32_t row, int32_t palette)
{
    Message message;
    uint32_t i = 0;
    for (; text[i] != '\0' && i < MESSAGE_CHARS - 1u; i++)
        message.text[i] = text[i];
    message.text[i] = '\0';
    message.column = (int8_t)column;
    message.row = (int8_t)row;
    message.palette = (int8_t)palette;
    return message;
}

char *decimal(int32_t value, char *out)
{
    char digits[12];
    uint32_t n = 0;
    uint32_t v = value < 0 ? (uint32_t)-value : (uint32_t)value;
    do {
        digits[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v != 0u);
    uint32_t i = 0;
    if (value < 0)
        out[i++] = '-';
    while (n > 0u)
        out[i++] = digits[--n];
    out[i] = '\0';
    return out;
}

Message stage_message(int32_t stage)
{
    char text[MESSAGE_CHARS] = "STAGE ";
    decimal(stage, text + 6);
    return message_at(text, 10, 18, 0);
}

Messages messages_show(const Messages *messages, const Message *message)
{
    Messages next = no_messages();
    for (uint32_t i = 0; i < messages->count; i++) {
        const Message *m = &messages->list[i];
        if (m->row != message->row || m->column != message->column)
            next.list[next.count++] = *m;
    }
    if (next.count < MESSAGE_SLOTS)
        next.list[next.count++] = *message;
    return next;
}

Messages messages_erase_row(const Messages *messages, int32_t row)
{
    Messages next = no_messages();
    for (uint32_t i = 0; i < messages->count; i++)
        if (messages->list[i].row != row)
            next.list[next.count++] = messages->list[i];
    return next;
}

int messages_shown_at(const Messages *messages, int32_t column, int32_t row)
{
    for (uint32_t i = 0; i < messages->count; i++)
        if (messages->list[i].row == row && messages->list[i].column == column)
            return 1;
    return 0;
}
