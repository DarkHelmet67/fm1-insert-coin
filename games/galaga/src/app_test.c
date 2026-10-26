/* SPDX-License-Identifier: GPL-3.0-only */
#include "app.h"
#include "cartridge/cartridge.h"
#include "config.h"
#include "input/fm1_keys.h"
#include "palette.h"
#include "test/check.h"

/** The cartridge, from cartridge.c. */
extern const Cartridge GALAGA_CARTRIDGE;

static App app;

/** Whether any voice of the last frame sounds. */
static int voices_sound(void)
{
    for (int t = 0; t < WSG_TICKS; t++)
        for (int v = 0; v < WSG_VOICES; v++)
            if (app.voices.tick[t][v].volume != 0)
                return 1;
    return 0;
}

static void the_keys_move_and_fire_as_in_space_invaders(void)
{
    const uint32_t op1 = FM1_KEY_BIT(FM1_KEY_OP1), op4 = FM1_KEY_BIT(FM1_KEY_OP4);
    const uint32_t glo = FM1_KEY_BIT(FM1_KEY_GLO);
    CHECK_EQ(app_controls(op1, 0, 0).direction, -1);
    CHECK_EQ(app_controls(glo, 0, 0).direction, 1);
    CHECK_EQ(app_controls(op1 | glo, 0, 0).direction, 0);
    CHECK(app_controls(op4, op4, 0).fire);
    CHECK(!app_controls(op4, 0, 0).fire); /* held, not pressed: one press, one rocket */
    CHECK(app_controls(0, 0, 1u << COIN_BUTTON).coin);
}

static void the_attract_screen_is_silent_and_the_coin_starts_the_theme(void)
{
    app_start(&app, 1234);
    for (int i = 0; i < 60; i++)
        app_step(&app, 0, 0, 0);
    CHECK(!voices_sound());
    CHECK_EQ(app.game.hi_score, 1234);
    app_step(&app, 0, 0, 1u << COIN_BUTTON);
    for (int i = 0; i < 30; i++)
        app_step(&app, 0, 0, 0);
    CHECK_EQ(app.game.screen, SCREEN_PLAYING);
    CHECK(GALAGA_CARTRIDGE.playing(&app));
    CHECK(voices_sound());
    CHECK_EQ(app.sounds, 0u);
}

static void the_colours_come_from_the_proms(void)
{
    app_start(&app, 0);
    CHECK_EQ(app.colors[COLOR_BACKGROUND], 0);
    CHECK(app.colors[SCREEN_COLOR_BASE + 1] != 0);
}

static void five_rows_of_six_fill_the_display(void)
{
    const ScreenMap map = GALAGA_CARTRIDGE.screen_map();
    int skipped_5 = 1, used = 1;
    for (uint32_t y = 0; y < SCREEN_SIZE; y++) {
        used &= map.source_row[y] != SCREEN_NO_ROW;
        skipped_5 &= map.source_row[y] % 6 != 5;
    }
    CHECK(used);
    CHECK(skipped_5);
    CHECK_EQ(map.source_row[0], 0);
    CHECK_EQ(map.source_row[SCREEN_SIZE - 1], 286);
    CHECK_EQ(map.width, 224);
}

int main(void)
{
    RUN(the_keys_move_and_fire_as_in_space_invaders);
    RUN(the_attract_screen_is_silent_and_the_coin_starts_the_theme);
    RUN(the_colours_come_from_the_proms);
    RUN(five_rows_of_six_fill_the_display);
    return check_report();
}
