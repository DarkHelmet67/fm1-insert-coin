/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"
#include "config.h"
#include "sounds.h"
#include "test/check.h"

/** About 1 KB each: kept out of the stack. */
static App app;

static void play_starts_a_game_with_the_coin_chirp(void)
{
    app = app_initial(0);
    app = app_step(&app, 0, 0, 1u << COIN_BUTTON);
    CHECK_EQ(app.game.screen, SCREEN_PLAYING);
    CHECK_EQ(app.sounds, SOUND_BIT(SOUND_COIN));
}

static void fx_switches_the_colours_on_any_screen(void)
{
    app = app_initial(0);
    app = app_step(&app, 0, 0, 1u << COLOR_BUTTON);
    CHECK(app.mono);
    app = app_step(&app, 0, 0, 1u << COLOR_BUTTON);
    CHECK(!app.mono);
}

static void a_fire_key_pressed_fires(void)
{
    const uint32_t op4 = FM1_KEY_BIT(FM1_KEY_OP4);
    app = app_initial(0);
    app = app_step(&app, 0, 0, 1u << COIN_BUTTON);
    app = app_step(&app, op4, op4, 0);
    CHECK(app.game.playing.shot.active);
    CHECK(app.sounds & SOUND_BIT(SOUND_SHOT));
}

static void the_record_is_saved_after_the_game_only(void)
{
    app = app_initial(0);
    app = app_step(&app, 0, 0, 1u << COIN_BUTTON);
    app.game.hi_score = 500;
    CHECK(!app_record_to_save(&app, 0));
    app.game.screen = SCREEN_GAME_OVER;
    CHECK(app_record_to_save(&app, 0));
    CHECK(!app_record_to_save(&app, 500));
}

int main(void)
{
    RUN(play_starts_a_game_with_the_coin_chirp);
    RUN(fx_switches_the_colours_on_any_screen);
    RUN(a_fire_key_pressed_fires);
    RUN(the_record_is_saved_after_the_game_only);
    return check_report();
}
