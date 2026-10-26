/* SPDX-License-Identifier: GPL-3.0-only */
#include "fighter.h"
#include "objects.h"
#include "phase.h"
#include "playing.h"
#include "program.h"
#include "sound_names.h"
#include "test/check.h"
#include "update_playing.h"

static PlayingState state;

/** What the simulated game went through. */
static int dived, bombed, lost;
static int32_t lost_code;

/** 6000 frames of play, firing every 8 frames and sweeping left and right. */
static void simulate(void)
{
    empty_playing_state(&state, STEP_INTRO);
    start_intro(&state);
    dived = bombed = lost = 0;
    for (int32_t f = 0; f < 6000; f++) {
        const int single = !state.dual;
        const Controls controls = {(int8_t)(((f >> 7) & 1) == 0 ? 1 : -1), (uint8_t)(f % 8 == 0),
                                   0};
        update_playing(&state, &controls);
        dived |= (state.sounds & SOUND_DIVE) != 0;
        for (int32_t b = 0; b < BOMB_COUNT; b++)
            bombed |= object_at(&state.objects, FIRST_BOMB + 2 * b)->x != 0;
        /* a double fighter that loses one of its ships flies on: look for a single one */
        if (!lost && single && (state.sounds & SOUND_FIGHTER_EXPLOSION)) {
            lost = 1;
            lost_code = object_at(&state.objects, FIGHTER)->code;
        }
    }
}

static void a_simulated_game_scores_by_shooting_aliens(void)
{
    simulate();
    CHECK(state.score > 0);
    CHECK(state.hits > 0);
}

static void aliens_dive_and_drop_bombs(void)
{
    CHECK(dived);
    CHECK(bombed);
}

static void the_fighter_is_lost_and_explodes(void)
{
    CHECK(lost);
    CHECK_EQ(lost_code, 0x20);
}

int main(void)
{
    RUN(a_simulated_game_scores_by_shooting_aliens);
    RUN(aliens_dive_and_drop_bombs);
    RUN(the_fighter_is_lost_and_explodes);
    return check_report();
}
