/* SPDX-License-Identifier: GPL-3.0-only */
#include "attack_waves.h"

#include "random.h"
#include "rom.h"
#include "stage_setup.h"
#include "table.h"

/** An empty place in the 16-byte buffer of one wave. */
#define FREE 0xff

/** The places of one wave: 8 on the left, 8 on the right. */
#define BUFFER_SIZE 16

/** The stage that gives the waves: past stage 22 the last four repeat. */
static int32_t wave_stage(int32_t stage)
{
    int32_t over = stage - 0x16;
    int32_t repeat = over > 0 ? (over + 3) / 4 : 0;
    return stage - repeat * 4;
}

/** The stage's 18 bytes in the combat or the challenge table. */
typedef struct {
    const uint8_t *table;
    uint32_t size;
    int32_t offset;
} StageRow;

/** Where the stage's bytes are. */
static StageRow stage_row(int32_t stage, int32_t rank)
{
    StageRow row;
    int32_t level = wave_stage(stage);
    if (is_challenge_stage(level)) {
        row.table = CHALLENGE_STAGES;
        row.size = CHALLENGE_STAGES_SIZE;
        row.offset = TABLE(CHALLENGE_STAGE_INDEX, (stage >> 2) & 7);
    } else {
        int32_t index = level - (level >> 2) - 1;
        row.table = COMBAT_STAGES;
        row.size = COMBAT_STAGES_SIZE;
        row.offset = TABLE(COMBAT_STAGE_INDEX, rank * 17 + index);
    }
    return row;
}

/** Byte `i` of the stage's row, `fallback` outside the table. */
static int32_t row_byte(const StageRow *row, int32_t i, int32_t fallback)
{
    return table_at(row->table, row->size, row->offset + i, fallback);
}

/**
 * `l_2612`: places the transients of a wave in random free places of the buffer. The low nibble
 * of `spec` is how many, the high bits (one per transient) whether it is a butterfly or a bee.
 * Random bytes are drawn from number `draw` on.
 */
static void place_transients(uint8_t buffer[BUFFER_SIZE], int32_t spec, int32_t seed, int32_t draw)
{
    int32_t count = spec & 0x0f;
    int32_t range = (count >> 1) + 4;
    for (int32_t i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = FREE;
    for (int32_t b = count; b >= 1; b--) {
        /* Each transient retries until it finds a free place, like the original. */
        int32_t position;
        do {
            position = (random_byte(seed, draw++) % range) | ((b & 1) == 1 ? 8 : 0);
        } while (buffer[position] != FREE);
        int butterfly = ((spec << (count - b)) & 0x80) != 0;
        buffer[position] = (uint8_t)(((b << 1) & 0xff) | (butterfly ? 0x40 : 0) | 0x38);
    }
}

/** `l_2636`: the 8 aliens of wave `wave` fill the free places, 4 in each half of the buffer. */
static void place_aliens(uint8_t buffer[BUFFER_SIZE], int32_t wave)
{
    for (int32_t n = 0; n < 8; n++) {
        int32_t from = n < 4 ? 0 : 8;
        for (int32_t i = from; i < BUFFER_SIZE; i++) {
            if (buffer[i] == FREE) {
                buffer[i] = (uint8_t)TABLE(WAVE_OBJECTS, wave * 8 + n);
                break;
            }
        }
    }
}

/** Adds a token to the list. */
static void add_token(StageWaves *waves, int32_t kind, int32_t control, int32_t object)
{
    WaveToken *token = &waves->tokens[waves->count++];
    token->kind = (uint8_t)kind;
    token->control = (uint8_t)control;
    token->object = (uint8_t)object;
}

/**
 * `l_2662`: aliens fly in pairs, one from each half, with their own control bytes. Returns the
 * control byte of the last left one, or -1 when the wave is empty.
 */
static int32_t pair_up(StageWaves *waves, const uint8_t buffer[BUFFER_SIZE], int32_t left,
                       int32_t right)
{
    int32_t last = -1;
    for (int32_t i = 0; i < 8 && buffer[i] != FREE; i++) {
        add_token(waves, TOKEN_ENTRY, left, buffer[i]);
        add_token(waves, TOKEN_ENTRY, right, buffer[i + 8]);
        last = left;
    }
    return last;
}

void build_attack_waves(StageWaves *waves, int32_t stage, int32_t rank, int32_t seed,
                        int rogue_fighter)
{
    StageRow row = stage_row(stage, rank);
    int32_t last = -1;
    waves->count = 0;
    for (int32_t w = 0; w < 5; w++) {
        uint8_t buffer[BUFFER_SIZE];
        place_transients(buffer, row_byte(&row, 2 + w * 3, 0), seed, w * 64);
        place_aliens(buffer, w);
        add_token(waves, TOKEN_START, 0, 0);
        last = pair_up(waves, buffer, row_byte(&row, 3 + w * 3, 0), row_byte(&row, 4 + w * 3, 0));
    }
    if (rogue_fighter && !is_challenge_stage(stage) && last >= 0)
        add_token(waves, TOKEN_ENTRY, last, 0x04);
    add_token(waves, TOKEN_END, 0, 0);
    waves->bomb_interval = (uint8_t)row_byte(&row, 0, 0x14);
    waves->entry_bomb_bits = (uint8_t)row_byte(&row, 1, 0);
}
