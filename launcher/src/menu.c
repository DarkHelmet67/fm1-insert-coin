/* SPDX-License-Identifier: GPL-3.0-only */
#include "menu.h"

#include "config.h"
#include "font/arcade_font.h"
#include "screen/draw.h"
#include "screen/thumbnail.h"

/** The rows of the menu, top to bottom. */
enum {
    TITLE_Y = 4,
    PICTURE_TOP = 16, /* the picture is centred in rows 16-159: Galaga's 144 rows fill them */
    PICTURE_HEIGHT = 144,
    NAME_Y = 166,
    HI_SCORE_Y = 178,
    DOTS_Y = 190,
    CHOOSE_Y = 202,
    PLAY_Y = 214,
    BACK_Y = 226,
};

/** The arrows each side of the name, and the dots: their size and spacing. */
enum { ARROW_MARGIN = 12, DOT_SIZE = 4, DOT_STEP = 10 };

/** PLAY shows for 40 frames out of 60. */
enum { BLINK_FRAMES = 60, BLINK_ON = 40 };

void menu_format_score(int32_t score, char out[7])
{
    int32_t rest = score < 0 ? 0 : score;
    for (int32_t i = 5; i >= 0; i--) {
        out[i] = (char)('0' + rest % 10);
        rest /= 10;
    }
    out[6] = '\0';
}

void menu_colors(const uint16_t game_colors[FB_COLORS], uint16_t out[FB_COLORS])
{
    for (uint32_t i = 0; i < FB_COLORS; i++)
        out[i] = i < MENU_GAME_COLORS ? game_colors[i] : MENU_COLORS[i];
}

/** A rectangle's outline, one pixel thick. */
static void draw_outline(Framebuffer *fb, int32_t x, int32_t y, int32_t w, int32_t h, uint8_t color)
{
    fb_fill_rect(fb, x, y, w, 1, color);
    fb_fill_rect(fb, x, y + h - 1, w, 1, color);
    fb_fill_rect(fb, x, y, 1, h, color);
    fb_fill_rect(fb, x + w - 1, y, 1, h, color);
}

/** The game's screen at half size, centred in the picture's rows, with a frame around it. */
static void draw_picture(Framebuffer *fb, const Framebuffer *game)
{
    const int32_t w = game->width / 2;
    const int32_t h = game->height / 2;
    const int32_t x = ((int32_t)MENU_SIZE - w) / 2;
    const int32_t y = PICTURE_TOP + (PICTURE_HEIGHT - h) / 2;
    thumbnail_draw(fb, game, x, y);
    draw_outline(fb, x - 2, y - 2, w + 4, h + 4, MENU_COLOR_FRAME);
}

/** One dot per game, the chosen one lit. */
static void draw_dots(Framebuffer *fb, uint32_t selected, uint32_t count)
{
    const int32_t width = (int32_t)count * DOT_STEP - (DOT_STEP - DOT_SIZE);
    const int32_t left = ((int32_t)MENU_SIZE - width) / 2;
    for (uint32_t i = 0; i < count; i++)
        fb_fill_rect(fb, left + (int32_t)i * DOT_STEP, DOTS_Y, DOT_SIZE, DOT_SIZE,
                     i == selected ? MENU_COLOR_TEXT : MENU_COLOR_DIM);
}

void menu_render(Framebuffer *fb, const MenuView *view, const Framebuffer *game)
{
    char digits[7];
    char line[24] = "HI-SCORE ";
    fb_clear(fb, MENU_COLOR_BACKGROUND);
    fb_draw_text_centered(fb, &ARCADE_FONT, MENU_TITLE, TITLE_Y, MENU_COLOR_TITLE);
    draw_picture(fb, game);
    fb_draw_text_centered(fb, &ARCADE_FONT, view->name, NAME_Y, MENU_COLOR_TEXT);
    menu_format_score(view->hi_score, digits);
    for (uint32_t i = 0; i < 7u; i++)
        line[9u + i] = digits[i];
    fb_draw_text_centered(fb, &ARCADE_FONT, line, HI_SCORE_Y, MENU_COLOR_DIM);
    if (view->count > 1u) {
        const int32_t right = (int32_t)MENU_SIZE - ARROW_MARGIN - ARCADE_FONT.glyph_width;
        fb_draw_text(fb, &ARCADE_FONT, "<", ARROW_MARGIN, NAME_Y, MENU_COLOR_FRAME);
        fb_draw_text(fb, &ARCADE_FONT, ">", right, NAME_Y, MENU_COLOR_FRAME);
        draw_dots(fb, view->selected, view->count);
        fb_draw_text_centered(fb, &ARCADE_FONT, MENU_CHOOSE_TEXT, CHOOSE_Y, MENU_COLOR_DIM);
    }
    if (view->frames % BLINK_FRAMES < BLINK_ON)
        fb_draw_text_centered(fb, &ARCADE_FONT, MENU_PLAY_TEXT, PLAY_Y, MENU_COLOR_TEXT);
    fb_draw_text_centered(fb, &ARCADE_FONT, MENU_BACK_TEXT, BACK_Y, MENU_COLOR_DIM);
}
