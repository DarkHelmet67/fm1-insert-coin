/* SPDX-License-Identifier: GPL-3.0-only */
#include "console.h"

#include "config.h"
#include "menu.h"

/** A panel where nothing happens: the attract screen behind the menu's picture runs on its own. */
static const Panel NO_PANEL = {0u, 0u, 0u, 0u, {0}};

const Cartridge *console_cartridge(const Console *console)
{
    return console->cartridges[console->launcher.selected];
}

int console_in_menu(const Console *console)
{
    return console->launcher.screen == LAUNCHER_MENU;
}

/** Switches the chosen game on, at its attract screen, with its best score. */
static void power_on(Console *console)
{
    const Cartridge *cartridge = console_cartridge(console);
    cartridge->start(console->state.bytes, records_get(&console->records, cartridge->record_tag));
}

void console_start(Console *console, const Cartridge *const *cartridges, uint32_t count,
                   const Records *records)
{
    console->launcher = launcher_initial(count, 0u);
    console->cartridges = cartridges;
    console->records = *records;
    console->sounds = 0;
    power_on(console);
}

/**
 * A best score beaten is kept once the game is over, as the cabinets wrote it.
 * @param console the launcher
 * @param left    the game is being left: over, even if it was still being played
 */
static void keep_record(Console *console, int left)
{
    const Cartridge *cartridge = console_cartridge(console);
    const void *state = console->state.bytes;
    const int32_t score = cartridge->hi_score(state);
    if ((left || !cartridge->playing(state)) &&
        score > records_get(&console->records, cartridge->record_tag))
        console->records = records_set(&console->records, cartridge->record_tag, score);
}

void console_step(Console *console, const Panel *panel)
{
    const Launcher before = console->launcher;
    console->launcher = launcher_step(&before, panel);
    const Launcher *after = &console->launcher;
    const Cartridge *cartridge = console_cartridge(console);
    console->sounds = 0;
    if (after->selected != before.selected) {
        power_on(console);
    } else if (after->screen == LAUNCHER_MENU && before.screen == LAUNCHER_GAME) {
        keep_record(console, 1); /* a game left halfway: its score may still be a record */
        power_on(console);
    } else if (after->screen == LAUNCHER_MENU) {
        cartridge->step(console->state.bytes, &NO_PANEL); /* silent, behind the picture */
    } else if (before.screen == LAUNCHER_GAME) {
        cartridge->step(console->state.bytes, panel); /* not on the frame PLAY left the menu */
        console->sounds = cartridge->sounds(console->state.bytes);
        keep_record(console, 0);
    }
}

uint32_t console_width(const Console *console)
{
    return console_in_menu(console) ? MENU_SIZE : console_cartridge(console)->width;
}

uint32_t console_height(const Console *console)
{
    return console_in_menu(console) ? MENU_SIZE : console_cartridge(console)->height;
}

ScreenMap console_screen_map(const Console *console)
{
    return console_in_menu(console) ? screen_map_centered(MENU_SIZE, MENU_SIZE)
                                    : console_cartridge(console)->screen_map();
}

void console_colors(const Console *console, uint16_t out[FB_COLORS])
{
    const uint16_t *game = console_cartridge(console)->colors(console->state.bytes);
    if (console_in_menu(console)) {
        menu_colors(game, out);
        return;
    }
    for (uint32_t i = 0; i < FB_COLORS; i++)
        out[i] = game[i];
}

void console_render(const Console *console, Framebuffer *screen, Framebuffer *game)
{
    const Cartridge *cartridge = console_cartridge(console);
    if (!console_in_menu(console)) {
        cartridge->render(console->state.bytes, screen);
        return;
    }
    const Launcher *launcher = &console->launcher;
    const MenuView view = {cartridge->name, records_get(&console->records, cartridge->record_tag),
                           launcher->selected, launcher->count, launcher->menu_frames};
    fb_init(game, cartridge->width, cartridge->height);
    cartridge->render(console->state.bytes, game);
    menu_render(screen, &view, game);
}

uint32_t console_keys(const Console *console)
{
    return console_in_menu(console) ? 0u : console_cartridge(console)->keys();
}

uint32_t console_buttons(const Console *console)
{
    return console_in_menu(console) ? 0u : console_cartridge(console)->buttons;
}

int console_waiting(const Console *console)
{
    return console_in_menu(console) || !console_cartridge(console)->playing(console->state.bytes);
}
