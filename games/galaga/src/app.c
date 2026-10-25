/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"

#include "config.h"
#include "palette.h"
#include "sound_board.h"
#include "sound_commands.h"

void app_start(App *app, int32_t hi_score)
{
    create_game(&app->game, hi_score);
    app->sound = new_sound_state();
    app->voices = wsg_silent_frame();
    app->sounds = 0;
    galaga_colors(app->colors);
}

Controls app_controls(uint32_t notes, uint32_t note_presses, uint32_t button_presses)
{
    const uint32_t held = keymap_actions(&CONTROLS, notes);
    const uint32_t pressed = keymap_actions(&CONTROLS, note_presses);
    Controls controls;
    controls.direction = (int8_t)((held & ACTION_LEFT) ? -1 : (held & ACTION_RIGHT) ? 1 : 0);
    controls.fire = (pressed & ACTION_FIRE) != 0u;
    controls.coin = (button_presses >> COIN_BUTTON & 1u) != 0u;
    return controls;
}

void app_step(App *app, uint32_t notes, uint32_t note_presses, uint32_t button_presses)
{
    const Controls controls = app_controls(notes, note_presses, button_presses);
    SoundCommands commands;
    update_game(&app->game, &controls);
    /* as main.ts: the sound program hears the game only while it is played */
    if (app->game.screen == SCREEN_PLAYING)
        sound_commands(&app->game.on.playing, &commands);
    else
        no_sound_commands(&commands);
    sound_frame(&app->sound, &commands, &app->voices);
    app->sounds = commands.explosion ? 1u : 0u;
}
