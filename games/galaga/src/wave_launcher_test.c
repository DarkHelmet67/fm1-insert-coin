/* SPDX-License-Identifier: GPL-3.0-only */
#include "stage_setup.h"
#include "test/check.h"
#include "wave_launcher.h"

static Launcher launcher;
static Slots slots;
static Objects objects;
static Looks looks;

/** A launcher waiting at a start mark, nothing flying; game timer 0 at 2, still running. */
static LauncherInputs set_up(void)
{
    StageWaves waves;
    waves.count = 2;
    waves.tokens[0].kind = TOKEN_START;
    waves.tokens[1].kind = TOKEN_END;
    start_launcher(&launcher, &waves);
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        slots.list[i] = free_slot();
    objects = empty_objects();
    looks = no_looks();
    const LauncherInputs inputs = {0, 1, 0, 0, 2, &looks, 0};
    return inputs;
}

static void a_normal_stage_starts_the_next_wave_once_nothing_flies(void)
{
    const LauncherInputs inputs = set_up();
    run_launcher(&launcher, &slots, &objects, &inputs);
    CHECK_EQ(launcher.wave_count, 1);
}

static void a_challenging_stage_waits_for_game_timer_0(void)
{
    LauncherInputs inputs = set_up();
    inputs.challenge = 1;
    run_launcher(&launcher, &slots, &objects, &inputs);
    CHECK_EQ(launcher.wave_count, 0);
    inputs.timer = 0;
    run_launcher(&launcher, &slots, &objects, &inputs);
    CHECK_EQ(launcher.wave_count, 1);
}

static void the_next_wave_waits_while_aliens_fly_resetting_the_pause(void)
{
    LauncherInputs inputs = set_up();
    inputs.flying = 1;
    const LauncherReport report = run_launcher(&launcher, &slots, &objects, &inputs);
    CHECK_EQ(launcher.wave_count, 0);
    CHECK_EQ(report.timer, 2);
}

int main(void)
{
    RUN(a_normal_stage_starts_the_next_wave_once_nothing_flies);
    RUN(a_challenging_stage_waits_for_game_timer_0);
    RUN(the_next_wave_waits_while_aliens_fly_resetting_the_pause);
    return check_report();
}
