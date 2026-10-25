/* SPDX-License-Identifier: GPL-3.0-only */
#include "endings.h"

#include "extra_lives.h"

/** Aliens of a challenging stage: all 40 shot is a "perfect". */
#define CHALLENGE_ALIENS 40

/** The actions of the script lines. */
typedef enum {
    ACTION_NOTHING,       /**< the line only waits */
    ACTION_TUNE,          /**< the results start with their tune */
    ACTION_HITS_TEXT,     /**< "NUMBER OF HITS" */
    ACTION_HITS_NUMBER,   /**< ... and the number */
    ACTION_BONUS_TEXT,    /**< "BONUS" */
    ACTION_BONUS_POINTS,  /**< 100 points a hit, written and added */
    ACTION_ERASE_RESULTS, /**< the rows of the results are erased */
    ACTION_PERFECT_ON,    /**< "PERFECT !" shown ... */
    ACTION_PERFECT_OFF,   /**< ... and erased: it blinks */
    ACTION_SPECIAL_BONUS, /**< "SPECIAL BONUS 10000 PTS", added */
    ACTION_GAME_OVER,     /**< "GAME OVER" */
    ACTION_RESULTS,       /**< the screen clears and the results of the game appear */
    ACTION_CLEAR_TEXTS,   /**< the results go away */
} Action;

/** One line of a script. */
typedef struct {
    uint8_t action; /**< an Action */
    uint8_t wait;   /**< a Wait */
} ScriptLine;

/** The challenge stage results: with fewer than 40 hits, a bonus of 100 points per hit. */
static const ScriptLine CHALLENGE_SCRIPT[] = {
    {ACTION_TUNE, WAIT_PAUSE},          {ACTION_HITS_TEXT, WAIT_PAUSE},
    {ACTION_HITS_NUMBER, WAIT_PAUSE},   {ACTION_BONUS_TEXT, WAIT_PAUSE},
    {ACTION_BONUS_POINTS, WAIT_PAUSE},  {ACTION_NOTHING, WAIT_PAUSE},
    {ACTION_ERASE_RESULTS, WAIT_PAUSE},
};

/**
 * All 40 aliens shot: "PERFECT !" blinks 7 times (`l_0699`), a phase every 16 frames, ending
 * shown; then a special bonus of 10000 points.
 */
static const ScriptLine PERFECT_SCRIPT[] = {
    {ACTION_TUNE, WAIT_PAUSE},        {ACTION_HITS_TEXT, WAIT_PAUSE},
    {ACTION_HITS_NUMBER, WAIT_PAUSE}, {ACTION_NOTHING, WAIT_BLINK},
    {ACTION_PERFECT_ON, WAIT_BLINK},  {ACTION_PERFECT_OFF, WAIT_BLINK},
    {ACTION_PERFECT_ON, WAIT_BLINK},  {ACTION_PERFECT_OFF, WAIT_BLINK},
    {ACTION_PERFECT_ON, WAIT_BLINK},  {ACTION_PERFECT_OFF, WAIT_BLINK},
    {ACTION_PERFECT_ON, WAIT_BLINK},  {ACTION_SPECIAL_BONUS, WAIT_PAUSE},
    {ACTION_NOTHING, WAIT_PAUSE},     {ACTION_ERASE_RESULTS, WAIT_PAUSE},
};

/** `l_04FD`: "GAME OVER", then the screen clears and the results of the game appear. */
static const ScriptLine GAME_OVER_SCRIPT[] = {
    {ACTION_GAME_OVER, WAIT_PAUSE}, {ACTION_NOTHING, WAIT_PAUSE},     {ACTION_NOTHING, WAIT_BEAM},
    {ACTION_RESULTS, WAIT_RESULTS}, {ACTION_CLEAR_TEXTS, WAIT_PAUSE},
};

/** The lines of script `name`, and how many through `length`. */
static const ScriptLine *script_lines(int32_t name, int32_t *length)
{
    switch (name) {
    case SCRIPT_CHALLENGE:
        *length = (int32_t)(sizeof CHALLENGE_SCRIPT / sizeof CHALLENGE_SCRIPT[0]);
        return CHALLENGE_SCRIPT;
    case SCRIPT_PERFECT:
        *length = (int32_t)(sizeof PERFECT_SCRIPT / sizeof PERFECT_SCRIPT[0]);
        return PERFECT_SCRIPT;
    default:
        *length = (int32_t)(sizeof GAME_OVER_SCRIPT / sizeof GAME_OVER_SCRIPT[0]);
        return GAME_OVER_SCRIPT;
    }
}

int32_t script_length(int32_t name)
{
    int32_t length;
    script_lines(name, &length);
    return length;
}

Wait script_wait(int32_t name, int32_t line)
{
    int32_t length;
    const ScriptLine *lines = script_lines(name, &length);
    return line < length ? (Wait)lines[line].wait : WAIT_PAUSE;
}

/** Shows `text` at a cell. */
static void show_text(PlayingState *state, const char *text, int32_t column, int32_t row,
                      int32_t palette)
{
    const Message message = message_at(text, column, row, palette);
    state->messages = messages_show(&state->messages, &message);
}

/** Shows `value` at a cell. */
static void show_number(PlayingState *state, int32_t value, int32_t column, int32_t row,
                        int32_t palette)
{
    char text[12];
    show_text(state, decimal(value, text), column, row, palette);
}

/** Erases the row of one of the texts of the main program. */
static void erase_row_of(PlayingState *state, MessageName name)
{
    state->messages = messages_erase_row(&state->messages, message_of(name).row);
}

char *hit_ratio(int32_t hits, int32_t shots, char *out)
{
    int32_t hundredths = shots == 0 ? 0 : hits * 10000 / shots;
    int32_t tenths = hundredths / 10 + (hundredths % 10 >= 5 ? 1 : 0);
    int32_t whole = tenths / 10;
    char *end = out;
    if (whole != 0) {
        decimal(whole, out);
        while (*end)
            end++;
    }
    end[0] = '.';
    end[1] = (char)('0' + tenths % 10);
    end[2] = ' ';
    end[3] = '%';
    end[4] = '\0';
    return out;
}

/** The screen clears and the results of the game appear. */
static void show_results(PlayingState *state)
{
    const Message shots = message_of(MESSAGE_SHOTS_FIRED);
    const Message hits = message_of(MESSAGE_RESULT_HITS);
    const Message ratio = message_of(MESSAGE_HIT_MISS_RATIO);
    char text[12];
    state->objects = empty_objects();
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        state->slots.list[i] = free_slot();
    state->messages = no_messages();
    show_message(state, MESSAGE_RESULTS);
    show_message(state, MESSAGE_SHOTS_FIRED);
    show_number(state, state->shots, 20, shots.row, shots.palette);
    show_message(state, MESSAGE_RESULT_HITS);
    show_number(state, state->hits, 20, hits.row, hits.palette);
    show_message(state, MESSAGE_HIT_MISS_RATIO);
    show_text(state, hit_ratio(state->hits, state->shots, text), 20, ratio.row, 3);
    state->star_control.ship_on_screen = 0;
}

/** The challenge bonus: 100 points a hit, written as hits followed by "00". */
static void bonus_points(PlayingState *state)
{
    char text[16];
    decimal(state->flying_hits, text);
    if (state->flying_hits != 0) {
        char *end = text;
        while (*end)
            end++;
        end[0] = '0';
        end[1] = '0';
        end[2] = '\0';
    }
    show_text(state, text, 15, 21, 0);
    add_points(state, state->flying_hits * 100);
}

void run_script_line(PlayingState *state, int32_t name, int32_t line)
{
    int32_t length;
    const ScriptLine *lines = script_lines(name, &length);
    if (line >= length)
        return;
    switch (lines[line].action) {
    case ACTION_TUNE:
        play(state, state->flying_hits == CHALLENGE_ALIENS ? SOUND_PERFECT : SOUND_CHALLENGE_END);
        break;
    case ACTION_HITS_TEXT:
        show_message(state, MESSAGE_NUMBER_OF_HITS);
        break;
    case ACTION_HITS_NUMBER:
        show_number(state, state->flying_hits, 21, message_of(MESSAGE_NUMBER_OF_HITS).row, 0);
        break;
    case ACTION_BONUS_TEXT:
        show_message(state, MESSAGE_BONUS);
        break;
    case ACTION_BONUS_POINTS:
        bonus_points(state);
        break;
    case ACTION_ERASE_RESULTS:
        erase_row_of(state, MESSAGE_PERFECT);
        erase_row_of(state, MESSAGE_NUMBER_OF_HITS);
        erase_row_of(state, MESSAGE_BONUS);
        break;
    case ACTION_PERFECT_ON:
        show_message(state, MESSAGE_PERFECT);
        break;
    case ACTION_PERFECT_OFF:
        erase_row_of(state, MESSAGE_PERFECT);
        break;
    case ACTION_SPECIAL_BONUS:
        show_message(state, MESSAGE_SPECIAL_BONUS);
        add_points(state, 10000);
        break;
    case ACTION_GAME_OVER:
        show_message(state, MESSAGE_GAME_OVER);
        break;
    case ACTION_RESULTS:
        show_results(state);
        break;
    case ACTION_CLEAR_TEXTS:
        state->messages = no_messages();
        break;
    default:
        break;
    }
}

void start_wait(PlayingState *state, Wait wait)
{
    if (wait == WAIT_PAUSE)
        state->timers = set_timer(&state->timers, 3, 3);
    else if (wait == WAIT_RESULTS)
        state->timers = set_timer(&state->timers, 2, 0x0e);
}

int wait_over(const PlayingState *state, Wait wait)
{
    switch (wait) {
    case WAIT_PAUSE:
        return state->timers.t[3] == 0;
    case WAIT_RESULTS:
        return state->timers.t[2] == 0;
    case WAIT_BLINK:
        return (state->frame & 0x0f) == 0;
    default:
        return (state->capture.tasks & CAPTURE_BEAM) == 0;
    }
}
