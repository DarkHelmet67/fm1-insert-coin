/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"
#include "menu.h"
#include "test/check.h"

static Framebuffer screen;
static Framebuffer game;

/** How many pixels of `color` the menu has in rows y0 to y1 (included). */
static uint32_t count_color(uint8_t color, int32_t y0, int32_t y1)
{
    uint32_t n = 0;
    for (int32_t y = y0; y <= y1; y++)
        for (int32_t x = 0; x < (int32_t)MENU_SIZE; x++)
            n += fb_get(&screen, x, y) == color;
    return n;
}

static void scores_have_six_digits(void)
{
    char digits[7];
    menu_format_score(1500, digits);
    CHECK(digits[0] == '0' && digits[2] == '1' && digits[5] == '0' && digits[6] == '\0');
    menu_format_score(-3, digits);
    CHECK(digits[5] == '0');
}

static void the_picture_is_the_game_at_half_size_centred(void)
{
    const MenuView view = {"SPACE INVADERS", 1500, 0, 3, 0};
    fb_init(&screen, MENU_SIZE, MENU_SIZE);
    fb_init(&game, 224, 256);
    fb_fill_rect(&game, 0, 0, 224, 256, 5);
    menu_render(&screen, &view, &game);
    /* 112 x 128 at (64, 24): rows 16-159 hold Galaga's 144, a shorter picture is centred */
    CHECK_EQ(fb_get(&screen, 64, 24), 5);
    CHECK_EQ(fb_get(&screen, 175, 151), 5);
    CHECK_EQ(fb_get(&screen, 63, 24), 0);
    CHECK_EQ(fb_get(&screen, 62, 22), MENU_COLOR_FRAME);
}

static void play_blinks(void)
{
    MenuView view = {"BREAKOUT", 0, 1, 3, 0};
    fb_init(&screen, MENU_SIZE, MENU_SIZE);
    fb_init(&game, 228, 208);
    menu_render(&screen, &view, &game);
    const uint32_t lit = count_color(MENU_COLOR_TEXT, 214, 220);
    view.frames = 50;
    menu_render(&screen, &view, &game);
    CHECK(lit > 0u);
    CHECK_EQ(count_color(MENU_COLOR_TEXT, 214, 220), 0);
}

static void one_game_needs_no_arrows_nor_dots(void)
{
    const MenuView view = {"SPACE INVADERS", 0, 0, 1, 0};
    fb_init(&screen, MENU_SIZE, MENU_SIZE);
    fb_init(&game, 224, 256);
    menu_render(&screen, &view, &game);
    CHECK_EQ(count_color(MENU_COLOR_FRAME, 166, 172), 0);
    CHECK_EQ(count_color(MENU_COLOR_DIM, 190, 208), 0);
}

static void the_palette_is_the_game_then_the_menu(void)
{
    uint16_t game_colors[FB_COLORS] = {0};
    uint16_t out[FB_COLORS];
    game_colors[3] = 0xABCD;
    game_colors[MENU_COLOR_TEXT] = 0x1111; /* past the 16 a game may use: ignored */
    menu_colors(game_colors, out);
    CHECK_EQ(out[3], 0xABCD);
    CHECK_EQ(out[MENU_COLOR_TEXT], MENU_COLORS[MENU_COLOR_TEXT]);
}

int main(void)
{
    RUN(scores_have_six_digits);
    RUN(the_picture_is_the_game_at_half_size_centred);
    RUN(play_blinks);
    RUN(one_game_needs_no_arrows_nor_dots);
    RUN(the_palette_is_the_game_then_the_menu);
    return check_report();
}
