/* SPDX-License-Identifier: GPL-3.0-only */
#include "clone_attack.h"
#include "objects.h"
#include "phase.h"
#include "playing.h"
#include "sound_names.h"
#include "stage_setup.h"
#include "test/check.h"

static PlayingState state;

/** Stage 4 with 5 aliens left, a bee resting at $0A, the task on. */
static void set_up(void)
{
    empty_playing_state(&state, STEP_RUNNING);
    state.stage = 4;
    state.params = stage_parameters(4, RANK_A);
    state.alive = 5;
    set_tasks(&state, TASK_CLONES | TASK_FIRE, 0);
    GameObject *bee = object_ref(&state.objects, 0x0a);
    bee->disposition = DISPOSITION_HOME;
    bee->code = 0x18;
    bee->color = 3;
    bee->x = 100;
    bee->y = 80;
}

/** `n` frames of the clone attack alone. */
static void run(int32_t n)
{
    for (int32_t i = 0; i < n; i++)
        clone_attack(&state);
}

static void it_waits_for_fewer_aliens_than_the_stage_parameter(void)
{
    set_up();
    state.alive = 10;
    clone_attack(&state);
    CHECK_EQ(state.attacks.bonus_bee_timer, 0);
    CHECK_EQ(state.sounds, 0u);
}

static void a_resting_bee_flashes_then_dives_with_the_triple_bonus_armed(void)
{
    set_up();
    clone_attack(&state);
    CHECK_EQ(state.attacks.bonus_bee, 0x0a);
    CHECK_EQ(state.attacks.bonus_bee_timer, 0xc0);
    CHECK_EQ(state.attacks.bonus_bee_color, 5);
    CHECK_EQ(state.sounds, SOUND_BONUS_BEE);
    run(16);
    CHECK_EQ(object_at(&state.objects, 0x0a)->color, 5);
    run(48);
    CHECK_EQ(object_at(&state.objects, 0x0a)->disposition, DISPOSITION_FLYING);
    CHECK_EQ(state.triple_left, 3);
    CHECK_EQ(state.triple_bonus.hundreds, 0x0a);
    CHECK_EQ(state.triple_bonus.code, 0xb8);
    CHECK(!task_on(&state, TASK_CLONES));
}

int main(void)
{
    RUN(it_waits_for_fewer_aliens_than_the_stage_parameter);
    RUN(a_resting_bee_flashes_then_dives_with_the_triple_bonus_armed);
    return check_report();
}
