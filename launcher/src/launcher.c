/* SPDX-License-Identifier: GPL-3.0-only */
#include "launcher.h"

#include "config.h"

Launcher launcher_initial(uint32_t count, uint32_t selected)
{
    Launcher launcher;
    launcher.screen = LAUNCHER_MENU;
    launcher.count = count;
    launcher.selected = selected < count ? selected : 0u;
    launcher.held_frames = 0;
    launcher.menu_frames = 0;
    return launcher;
}

int32_t launcher_menu_turns(const Panel *panel)
{
    int32_t turns = 0;
    for (uint32_t e = 0; e < FM1_ENCODER_COUNT; e++)
        if (MENU_ENCODERS >> e & 1u)
            turns += panel->turns[e];
    return turns;
}

uint32_t launcher_wrap(uint32_t selected, int32_t turns, uint32_t count)
{
    const int32_t n = (int32_t)count;
    const int32_t moved = ((int32_t)selected + turns % n + n) % n;
    return (uint32_t)moved;
}

/** In the menu: the encoders choose, PLAY starts. */
static Launcher menu_step(const Launcher *launcher, const Panel *panel)
{
    Launcher next = *launcher;
    next.menu_frames++;
    next.selected = launcher_wrap(launcher->selected, launcher_menu_turns(panel), launcher->count);
    if (panel->button_presses & FM1_BUTTON_BIT(START_BUTTON)) {
        next.screen = LAUNCHER_GAME;
        next.held_frames = 0;
    }
    return next;
}

/** In a game: PLAY held down for LEAVE_FRAMES goes back to the menu. */
static Launcher game_step(const Launcher *launcher, const Panel *panel)
{
    Launcher next = *launcher;
    next.held_frames =
        panel->buttons & FM1_BUTTON_BIT(START_BUTTON) ? launcher->held_frames + 1u : 0u;
    if (next.held_frames >= LEAVE_FRAMES) {
        next.screen = LAUNCHER_MENU;
        next.held_frames = 0;
        next.menu_frames = 0;
    }
    return next;
}

Launcher launcher_step(const Launcher *launcher, const Panel *panel)
{
    return launcher->screen == LAUNCHER_MENU ? menu_step(launcher, panel)
                                             : game_step(launcher, panel);
}
