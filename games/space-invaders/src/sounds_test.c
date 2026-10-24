/* SPDX-License-Identifier: GPL-3.0-only */
#include "sounds.h"
#include "test/check.h"

static const Controls IDLE = {0, 0, 0};
static const Controls FIRE = {0, 1, 0};
static const Controls COIN = {0, 0, 1};

/** Large structs: kept out of the stack. */
static GameState before, after;

/** A game just started, and the frame after it with `controls`. */
static void step_from_playing(const Controls *controls)
{
    before = game_initial(0);
    before = game_step(&before, &COIN);
    after = game_step(&before, controls);
}

static void a_coin_chirps(void)
{
    before = game_initial(0);
    after = game_step(&before, &COIN);
    CHECK_EQ(sounds_for(&before, &after), SOUND_BIT(SOUND_COIN));
}

static void firing_plays_the_laser(void)
{
    step_from_playing(&FIRE);
    CHECK(sounds_for(&before, &after) & SOUND_BIT(SOUND_SHOT));
    step_from_playing(&IDLE);
    CHECK(!(sounds_for(&before, &after) & SOUND_BIT(SOUND_SHOT)));
}

static void every_pass_of_the_formation_plays_the_next_march_note(void)
{
    step_from_playing(&IDLE);
    uint32_t notes = 0;
    for (int frame = 0; frame < 55 * 4; frame++) {
        before = after;
        after = game_step(&before, &IDLE);
        notes |= sounds_for(&before, &after);
    }
    CHECK(notes & SOUND_BIT(SOUND_MARCH_1));
    CHECK(notes & SOUND_BIT(SOUND_MARCH_2));
    CHECK(notes & SOUND_BIT(SOUND_MARCH_3));
}

static void beating_the_record_sounds_the_fanfare_once(void)
{
    before = game_initial(100);
    before = game_step(&before, &COIN);
    before.playing.score = 100;
    after = before;
    after.playing.score = 110;
    CHECK(sounds_for(&before, &after) & SOUND_BIT(SOUND_NEW_RECORD));
    before.playing.score = 110;
    after.playing.score = 120;
    CHECK(!(sounds_for(&before, &after) & SOUND_BIT(SOUND_NEW_RECORD)));
}

static void the_first_game_ever_has_no_fanfare(void)
{
    before = game_initial(0);
    before = game_step(&before, &COIN);
    after = before;
    after.playing.score = 10;
    CHECK(!(sounds_for(&before, &after) & SOUND_BIT(SOUND_NEW_RECORD)));
}

static void losing_the_cannon_explodes(void)
{
    step_from_playing(&IDLE);
    after = before;
    after.playing.cannon_explosion = 90;
    CHECK(sounds_for(&before, &after) & SOUND_BIT(SOUND_CANNON_HIT));
}

static void the_attract_screen_is_silent(void)
{
    before = game_initial(0);
    after = game_step(&before, &IDLE);
    CHECK_EQ(sounds_for(&before, &after), 0u);
}

static void every_effect_has_layers(void)
{
    for (uint32_t i = 0; i < SOUND_COUNT; i++)
        CHECK(SOUND_EFFECTS[i].count > 0u && SOUND_EFFECTS[i].layers != 0);
}

int main(void)
{
    RUN(a_coin_chirps);
    RUN(firing_plays_the_laser);
    RUN(every_pass_of_the_formation_plays_the_next_march_note);
    RUN(beating_the_record_sounds_the_fanfare_once);
    RUN(the_first_game_ever_has_no_fanfare);
    RUN(losing_the_cannon_explodes);
    RUN(the_attract_screen_is_silent);
    RUN(every_effect_has_layers);
    return check_report();
}
