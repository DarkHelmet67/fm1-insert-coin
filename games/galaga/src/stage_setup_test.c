/* SPDX-License-Identifier: GPL-3.0-only */
#include "stage_setup.h"
#include "test/check.h"

static void every_fourth_stage_from_stage_3_is_a_challenging_stage(void)
{
    CHECK(!is_challenge_stage(1));
    CHECK(!is_challenge_stage(2));
    CHECK(is_challenge_stage(3));
    CHECK(!is_challenge_stage(4));
    CHECK(is_challenge_stage(7));
    CHECK(is_challenge_stage(11));
}

/** How many objects a stage gives a look. */
static int32_t known_count(const Looks *looks)
{
    int32_t n = 0;
    for (int32_t i = 0; i < 64; i++)
        n += looks->known[i] != 0;
    return n;
}

static void bees_bosses_and_butterflies_get_their_codes_and_palettes(void)
{
    const Looks looks = stage_looks(1);
    CHECK_EQ(looks.look[0x08 >> 1].code, 0x18);
    CHECK_EQ(looks.look[0x08 >> 1].color, 3);
    CHECK_EQ(looks.look[0x30 >> 1].code, 0x08);
    CHECK_EQ(looks.look[0x30 >> 1].color, 0);
    CHECK_EQ(looks.look[0x5e >> 1].code, 0x10);
    CHECK_EQ(looks.look[0x5e >> 1].color, 2);
    CHECK_EQ(known_count(&looks), 44);
}

static void challenging_stages_use_the_special_aliens(void)
{
    CHECK_EQ(stage_looks(3).look[0x08 >> 1].code, 0x18);
    CHECK_EQ(stage_looks(7).look[0x08 >> 1].code, 0x10);
    CHECK_EQ(stage_looks(7).look[0x08 >> 1].color, 2);
    CHECK_EQ(stage_looks(11).look[0x08 >> 1].code, 0x68);
}

static void the_first_stage_of_rank_a_is_read_from_the_rom_table(void)
{
    const StageParameters p = stage_parameters(1, RANK_A);
    CHECK_EQ(p.boss_timer, 0);
    CHECK_EQ(p.red_timer, 0);
    CHECK_EQ(p.yellow_timer, 0);
    CHECK_EQ(p.max_bombers, 2);
    CHECK_EQ(p.max_bombers_later, 2);
    CHECK_EQ(p.beam_speed, 0x0c);
    CHECK_EQ(p.continuous_bombing, 6);
    CHECK_EQ(p.harder_entries, 0);
    CHECK_EQ(p.clone_attack, 0);
    CHECK_EQ(stage_parameters(4, RANK_A).clone_attack, 0x0a);
}

static void the_last_four_stages_repeat_after_stage_26(void)
{
    const StageParameters a = stage_parameters(29, RANK_B);
    const StageParameters b = stage_parameters(25, RANK_B);
    CHECK_EQ(a.max_bombers, b.max_bombers);
    CHECK_EQ(a.beam_speed, b.beam_speed);
    CHECK_EQ(a.clone_attack, b.clone_attack);
    CHECK_EQ(a.boss_timer, b.boss_timer);
}

static void the_group_bonus_and_the_star_speed_grow_with_the_stage(void)
{
    CHECK_EQ(challenge_bonus(3).tens, 10);
    CHECK_EQ(challenge_bonus(3).score_code, 0xb8);
    CHECK_EQ(challenge_bonus(35).tens, 30);
    CHECK_EQ(star_speed_target(1), 0x40);
    CHECK_EQ(star_speed_target(30), 0x80);
    CHECK_EQ(star_speed_target(4), 0x50);
}

int main(void)
{
    RUN(every_fourth_stage_from_stage_3_is_a_challenging_stage);
    RUN(bees_bosses_and_butterflies_get_their_codes_and_palettes);
    RUN(challenging_stages_use_the_special_aliens);
    RUN(the_first_stage_of_rank_a_is_read_from_the_rom_table);
    RUN(the_last_four_stages_repeat_after_stage_26);
    RUN(the_group_bonus_and_the_star_speed_grow_with_the_stage);
    return check_report();
}
