/* SPDX-License-Identifier: GPL-3.0-only */
#include "stage_setup.h"

#include "rom.h"
#include "table.h"

/** Bee (code $18, palette 3), Boss ($08, 0) and butterfly ($10, 2): the normal stages. */
#define BEE 0x1b
#define BOSS 0x08
#define BUTTERFLY 0x12

int is_challenge_stage(int32_t stage)
{
    return ((stage + 1) & 3) == 0;
}

Looks no_looks(void)
{
    Looks looks;
    for (uint32_t i = 0; i < 64u; i++) {
        const AlienLook none = {0, 0, 0};
        looks.look[i] = none;
        looks.known[i] = 0;
    }
    return looks;
}

/** Bit `index` of the "may drop bombs" table `d_2908`, most significant bit first. */
static int bomber_bit(int32_t index)
{
    return ((TABLE(BOMBER_BITS, index >> 3) << (index & 7)) & 0x80) != 0;
}

/** Code and palette packed as the program does: code in bits 3-6, palette in bits 0-2. */
static AlienLook unpack(int32_t packed, int bombs)
{
    const AlienLook look = {(uint8_t)(packed & 0x78), (uint8_t)(packed & 0x07), (uint8_t)bombs};
    return look;
}

Looks stage_looks(int32_t stage)
{
    const int32_t challenge = TABLE(CHALLENGE_LOOKS, (stage >> 2) & 7) >> 1;
    const int challenging = is_challenge_stage(stage);
    /* the three classes of aliens: first object, how many, and their packed look */
    const int32_t classes[3][3] = {
        {0x08, 20, challenging ? challenge : BEE},
        {0x30, 8, BOSS},
        {0x40, 16, challenging ? challenge : BUTTERFLY},
    };
    Looks looks = no_looks();
    int32_t before = 0;
    for (int c = 0; c < 3; c++) {
        for (int32_t i = 0; i < classes[c][1]; i++) {
            const int32_t id = classes[c][0] + i * 2;
            looks.look[id >> 1] = unpack(classes[c][2], bomber_bit(before + i));
            looks.known[id >> 1] = 1;
        }
        before += classes[c][1];
    }
    return looks;
}

ChallengeBonus challenge_bonus(int32_t stage)
{
    const int32_t index = stage >= 32 ? 3 : (stage >> 3) & 3;
    const ChallengeBonus bonus = {TABLE(CHALLENGE_BONUS, index * 2),
                                  TABLE(CHALLENGE_BONUS, index * 2 + 1)};
    return bonus;
}

/** The stage of the table: past stage 26 the last four repeat. */
static int32_t table_stage(int32_t stage)
{
    /* Math.ceil((stage - 0x1a) / 4), at least 0 */
    const int32_t over = stage - 0x1a;
    const int32_t repeat = over > 0 ? (over + 3) / 4 : 0;
    return stage - repeat * 4;
}

/** Where each rank's 130 bytes start (`bmbr_stg_cfg_lut`): rank A is the first block. */
static const int32_t RANK_BLOCK[4] = {1, 2, 3, 0};

StageParameters stage_parameters(int32_t stage, int32_t rank)
{
    const int32_t start = RANK_BLOCK[rank & 3] * 0x82 + (table_stage(stage) - 1) * 5;
    uint8_t n[10];
    for (int i = 0; i < 10; i++) {
        const int32_t byte = TABLE(STAGE_PARAMETERS, start + (i >> 1));
        n[i] = (uint8_t)((i & 1) == 0 ? byte >> 4 : byte & 0x0f);
    }
    StageParameters p;
    p.bomb_enable = n[0];
    p.boss_timer = n[1];
    p.red_timer = n[2];
    p.yellow_timer = n[3];
    p.max_bombers = n[4];
    p.max_bombers_later = n[5];
    p.beam_speed = n[6];
    p.continuous_bombing = n[7];
    p.harder_entries = n[8] != 0;
    p.harder_attacks = n[9] != 0;
    p.clone_attack = stage < 3 || is_challenge_stage(stage) ? 0 : 0x0a;
    return p;
}

int32_t star_speed_target(int32_t stage)
{
    return (((stage < 0x10 ? stage : 0x10) << 2) & 0x70) + 0x40;
}
