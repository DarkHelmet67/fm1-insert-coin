/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"
#include "launcher.h"
#include "test/check.h"

static const Panel NOTHING = {0u, 0u, 0u, 0u, {0}};

static Panel turned(uint32_t encoder, int32_t clicks)
{
    Panel panel = NOTHING;
    panel.turns[encoder] = clicks;
    return panel;
}

static void the_list_wraps_round_both_ways(void)
{
    CHECK_EQ(launcher_wrap(0, 1, 3), 1);
    CHECK_EQ(launcher_wrap(2, 1, 3), 0);
    CHECK_EQ(launcher_wrap(0, -1, 3), 2);
    CHECK_EQ(launcher_wrap(1, -7, 3), 0);
    CHECK_EQ(launcher_wrap(0, 5, 1), 0);
}

static void the_three_left_encoders_choose_the_game(void)
{
    Launcher launcher = launcher_initial(3, 0);
    const Panel select = turned(FM1_ENCODER_SELECT, 1);
    const Panel presets = turned(FM1_ENCODER_PRESETS, 1);
    const Panel algorithm = turned(FM1_ENCODER_ALGORITHM, -1);
    const Panel knob = turned(FM1_ENCODER_KNOB1, 1);
    launcher = launcher_step(&launcher, &select);
    CHECK_EQ(launcher.selected, 1);
    launcher = launcher_step(&launcher, &presets);
    CHECK_EQ(launcher.selected, 2);
    launcher = launcher_step(&launcher, &algorithm);
    CHECK_EQ(launcher.selected, 1);
    launcher = launcher_step(&launcher, &knob);
    CHECK_EQ(launcher.selected, 1);
}

static void play_starts_the_chosen_game(void)
{
    Launcher launcher = launcher_initial(3, 2);
    Panel play = NOTHING;
    play.buttons = play.button_presses = FM1_BUTTON_BIT(START_BUTTON);
    launcher = launcher_step(&launcher, &play);
    CHECK_EQ(launcher.screen, LAUNCHER_GAME);
    CHECK_EQ(launcher.selected, 2);
}

static void in_a_game_the_encoders_do_not_change_it(void)
{
    Launcher launcher = launcher_initial(3, 0);
    launcher.screen = LAUNCHER_GAME;
    const Panel select = turned(FM1_ENCODER_SELECT, 1);
    launcher = launcher_step(&launcher, &select);
    CHECK_EQ(launcher.selected, 0);
}

static void holding_play_goes_back_to_the_menu(void)
{
    Launcher launcher = launcher_initial(3, 1);
    launcher.screen = LAUNCHER_GAME;
    Panel held = NOTHING;
    held.buttons = FM1_BUTTON_BIT(START_BUTTON);
    for (uint32_t i = 1; i < LEAVE_FRAMES; i++)
        launcher = launcher_step(&launcher, &held);
    CHECK_EQ(launcher.screen, LAUNCHER_GAME);
    launcher = launcher_step(&launcher, &held);
    CHECK_EQ(launcher.screen, LAUNCHER_MENU);
    CHECK_EQ(launcher.selected, 1);
}

static void letting_go_of_play_starts_the_count_again(void)
{
    Launcher launcher = launcher_initial(1, 0);
    launcher.screen = LAUNCHER_GAME;
    Panel held = NOTHING;
    held.buttons = FM1_BUTTON_BIT(START_BUTTON);
    for (uint32_t i = 1; i < LEAVE_FRAMES; i++)
        launcher = launcher_step(&launcher, &held);
    launcher = launcher_step(&launcher, &NOTHING);
    launcher = launcher_step(&launcher, &held);
    CHECK_EQ(launcher.screen, LAUNCHER_GAME);
    CHECK_EQ(launcher.held_frames, 1);
}

int main(void)
{
    RUN(the_list_wraps_round_both_ways);
    RUN(the_three_left_encoders_choose_the_game);
    RUN(play_starts_the_chosen_game);
    RUN(in_a_game_the_encoders_do_not_change_it);
    RUN(holding_play_goes_back_to_the_menu);
    RUN(letting_go_of_play_starts_the_count_again);
    return check_report();
}
