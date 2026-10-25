/* SPDX-License-Identifier: GPL-3.0-only */
#include "attack_waves.h"
#include "stage_setup.h"
#include "test/check.h"

static StageWaves waves;

/** The aliens of the waves of `stage`, in order, into `objects`; returns how many. */
static int32_t entries(int32_t stage, int32_t seed, uint8_t *objects)
{
    int32_t n = 0;
    build_attack_waves(&waves, stage, RANK_A, seed, 0);
    for (int32_t i = 0; i < waves.count; i++)
        if (waves.tokens[i].kind == TOKEN_ENTRY)
            objects[n++] = waves.tokens[i].object;
    return n;
}

static void the_40_aliens_of_stage_1_come_in_five_waves_of_8(void)
{
    uint8_t objects[MAX_TOKENS];
    int32_t starts = 0;
    CHECK_EQ(entries(1, 1, objects), 40);
    for (int32_t i = 0; i < waves.count; i++)
        starts += waves.tokens[i].kind == TOKEN_START;
    CHECK_EQ(starts, 5);
    CHECK_EQ(waves.tokens[waves.count - 1].kind, TOKEN_END);
    CHECK_EQ(waves.tokens[1].control, 0x00);
    CHECK_EQ(waves.tokens[1].object, 0x58);
    CHECK_EQ(waves.tokens[2].control, 0xc0);
    CHECK_EQ(waves.tokens[2].object, 0x28);
}

static void special_enemies_join_from_stage_4(void)
{
    uint8_t objects[MAX_TOKENS];
    const int32_t n = entries(4, 1, objects);
    int32_t transients = 0;
    for (int32_t i = 0; i < n; i++)
        transients += (objects[i] & 0x38) == 0x38;
    /* stage 4 of rank A: 2 + 2 + 2 in waves 1, 4 and 5 */
    CHECK_EQ(transients, 6);
    CHECK_EQ(n, 46);
}

/** How many times each object appears. */
static void count(const uint8_t *objects, int32_t n, int32_t counts[256])
{
    for (int32_t i = 0; i < 256; i++)
        counts[i] = 0;
    for (int32_t i = 0; i < n; i++)
        counts[objects[i]]++;
}

static void the_random_bytes_only_choose_where_the_special_enemies_go(void)
{
    uint8_t a[MAX_TOKENS], b[MAX_TOKENS];
    int32_t ca[256], cb[256];
    const int32_t na = entries(9, 1, a);
    const int32_t nb = entries(9, 2, b);
    count(a, na, ca);
    count(b, nb, cb);
    int same = na == nb;
    for (int32_t i = 0; i < 256; i++)
        same &= ca[i] == cb[i];
    CHECK(same);
}

static void a_captured_fighter_flies_in_with_the_last_wave(void)
{
    build_attack_waves(&waves, 2, RANK_A, 1, 1);
    CHECK_EQ(waves.tokens[waves.count - 2].object, 0x04);
}

int main(void)
{
    RUN(the_40_aliens_of_stage_1_come_in_five_waves_of_8);
    RUN(special_enemies_join_from_stage_4);
    RUN(the_random_bytes_only_choose_where_the_special_enemies_go);
    RUN(a_captured_fighter_flies_in_with_the_last_wave);
    return check_report();
}
