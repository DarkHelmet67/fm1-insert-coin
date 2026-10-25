/* SPDX-License-Identifier: GPL-3.0-only */
#include "render.h"

#include "collisions.h"
#include "config.h"
#include "font/arcade_font.h"
#include "screen/draw.h"
#include "sprites.h"

/** Left edge of the "HI-SCORE" column in the header. */
#define HI_SCORE_X 88

/** Top of the bottom line with the reserve cannons, below the ground. */
#define RESERVE_Y (GROUND_Y + 4)

/** Frames each picture of the cannon explosion stays on screen before switching. */
#define CANNON_EXPLOSION_BLINK 5

/** Height of the "GAME OVER" message. */
#define GAME_OVER_Y 56

/** Writes a number in decimal, at least `digits` long with leading zeros. */
static void format_number(int32_t value, uint32_t digits, char out[12])
{
    char reversed[11];
    uint32_t n = 0;
    uint32_t v = value < 0 ? 0u : (uint32_t)value;
    do {
        reversed[n++] = (char)('0' + v % 10u);
        v /= 10u;
    } while (v && n < sizeof reversed);
    while (n < digits && n < sizeof reversed)
        reversed[n++] = '0';
    for (uint32_t i = 0; i < n; i++)
        out[i] = reversed[n - 1u - i];
    out[n] = '\0';
}

void format_score(int32_t score, char out[12])
{
    format_number(score, 4, out);
}

/** The colour of an invader of formation row `row` (1 = top). */
static uint8_t alien_color(uint32_t row)
{
    if (row <= 1u)
        return COLOR_ALIEN_TOP;
    return row <= 3u ? COLOR_ALIEN_MIDDLE : COLOR_ALIEN_BOTTOM;
}

/** The header: the player's score and the best score. */
static void render_header(Framebuffer *fb, int32_t score, int32_t hi_score)
{
    char digits[12];
    fb_draw_text(fb, &ARCADE_FONT, "SCORE<1>", 8, 8, COLOR_TEXT);
    format_score(score, digits);
    fb_draw_text(fb, &ARCADE_FONT, digits, 24, 20, COLOR_TEXT);
    fb_draw_text(fb, &ARCADE_FONT, "HI-SCORE", HI_SCORE_X, 8, COLOR_TEXT);
    format_score(hi_score, digits);
    fb_draw_text(fb, &ARCADE_FONT, digits, HI_SCORE_X + 16, 20, COLOR_TEXT);
}

/** The green ground line and, under it, the cannons left: a number and the waiting cannons. */
static void render_ground_and_lives(Framebuffer *fb, int32_t lives)
{
    char digits[12];
    fb_fill_rect(fb, 0, GROUND_Y, fb->width, 1, COLOR_CANNON);
    format_number(lives, 1, digits);
    fb_draw_text(fb, &ARCADE_FONT, digits, 8, RESERVE_Y, COLOR_CANNON);
    for (int32_t i = 0; i < lives - 1; i++)
        fb_draw_sprite(fb, &CANNON_SPRITE, 24 + i * 16, RESERVE_Y, COLOR_CANNON);
}

/** An explosion, a splash, or the points scored on the mystery ship. */
static void render_effect(Framebuffer *fb, const Effect *effect)
{
    char digits[12];
    switch (effect->kind) {
    case EFFECT_ALIEN:
        fb_draw_sprite(fb, &EXPLOSION_SPRITE, effect->x, effect->y, COLOR_TEXT);
        break;
    case EFFECT_SHOT:
        fb_draw_sprite(fb, &SHOT_EXPLOSION_SPRITE, effect->x, effect->y, COLOR_SHOTS);
        break;
    case EFFECT_BOMB:
        fb_draw_sprite(fb, &BOMB_EXPLOSION_SPRITE, effect->x, effect->y, COLOR_SHOTS);
        break;
    default:
        format_number(effect->points, 1, digits);
        fb_draw_text(fb, &ARCADE_FONT, digits, effect->x, effect->y, COLOR_UFO);
        break;
    }
}

/** The cannon, or its two-picture explosion while it is being destroyed. */
static void render_cannon(Framebuffer *fb, const PlayingState *state)
{
    const Sprite *sprite =
        state->cannon_explosion
            ? cannon_explosion_sprite(state->cannon_explosion / CANNON_EXPLOSION_BLINK)
            : &CANNON_SPRITE;
    fb_draw_sprite(fb, sprite, state->cannon_x, CANNON_Y, COLOR_CANNON);
}

/** The playfield: header, shields, invaders, mystery ship, shots, bombs and cannon. */
static void render_playing(Framebuffer *fb, const PlayingState *state, int32_t hi_score)
{
    render_header(fb, state->score, hi_score);
    for (uint32_t i = 0; i < SHIELD_COUNT; i++) {
        const Shield *shield = &state->shields.list[i];
        fb_draw_bitmap(fb, &shield->bitmap, shield->x, shield->y, COLOR_CANNON);
    }
    for (uint32_t i = 0; i < state->fleet.count; i++) {
        const Alien *alien = &state->fleet.aliens[i];
        fb_draw_sprite(fb, alien_sprite((AlienKind)alien->kind, alien->frame), alien->x, alien->y,
                       alien_color(alien->row));
    }
    if (state->ufo.flying)
        fb_draw_sprite(fb, &UFO_SPRITE, state->ufo.x, UFO_Y, COLOR_UFO);
    for (uint32_t i = 0; i < state->bombs.count; i++) {
        const Bomb *bomb = &state->bombs.active[i];
        fb_draw_sprite(fb, bomb_sprite((BombKind)bomb->kind, bomb->steps), bomb->x, bomb->y,
                       COLOR_SHOTS);
    }
    for (uint32_t i = 0; i < state->effects.count; i++)
        render_effect(fb, &state->effects.list[i]);
    if (state->shot.active)
        fb_draw_sprite(fb, &SHOT_SPRITE, state->shot.x, state->shot.y, COLOR_SHOTS);
    render_cannon(fb, state);
    render_ground_and_lives(fb, state->lives);
}

/** Horizontal centre of the sprite column and left edge of the text column of the score table. */
#define SPRITE_CENTER_X 72
#define LABEL_X 88

/** One line of the score table, with its top at y. */
static void render_score_row(Framebuffer *fb, const Sprite *sprite, uint8_t color,
                             const char *label, int32_t y)
{
    fb_draw_sprite(fb, sprite, SPRITE_CENTER_X - sprite->width / 2, y, color);
    fb_draw_text(fb, &ARCADE_FONT, label, LABEL_X, y, COLOR_TEXT);
}

/**
 * The attract screen: title, score table with invaders animated every half second, the
 * blinking "PRESS PLAY" (on for the first half of every second) and the keys.
 */
static void render_attract(Framebuffer *fb, uint32_t frames)
{
    const uint32_t frame = frames / 30u % 2u;
    fb_draw_text_centered(fb, &ARCADE_FONT, "SPACE INVADERS", 40, COLOR_TEXT);
    fb_draw_text_centered(fb, &ARCADE_FONT, "*SCORE ADVANCE TABLE*", 80, COLOR_TEXT);
    render_score_row(fb, &UFO_SPRITE, COLOR_UFO, "= ? MYSTERY", 104);
    render_score_row(fb, alien_sprite(ALIEN_SQUID, frame), alien_color(1), "= 30 POINTS", 124);
    render_score_row(fb, alien_sprite(ALIEN_CRAB, frame), alien_color(2), "= 20 POINTS", 144);
    render_score_row(fb, alien_sprite(ALIEN_OCTOPUS, frame), alien_color(4), "= 10 POINTS", 164);
    if (frames % 60u < 30u)
        fb_draw_text_centered(fb, &ARCADE_FONT, ATTRACT_PRESS_TEXT, ATTRACT_PRESS_Y, COLOR_TEXT);
    fb_draw_text_centered(fb, &ARCADE_FONT, ATTRACT_KEYS_TEXT, ATTRACT_KEYS_Y, COLOR_TEXT);
    fb_draw_text_centered(fb, &ARCADE_FONT, ATTRACT_COLOR_TEXT, ATTRACT_COLOR_Y, COLOR_TEXT);
}

void render_game(Framebuffer *fb, const GameState *state)
{
    fb_clear(fb, COLOR_BACKGROUND);
    switch (state->screen) {
    case SCREEN_PLAYING:
        render_playing(fb, &state->playing, state->hi_score);
        break;
    case SCREEN_GAME_OVER:
        render_playing(fb, &state->playing, state->hi_score);
        fb_draw_text_centered(fb, &ARCADE_FONT, "GAME OVER", GAME_OVER_Y, COLOR_UFO);
        break;
    default:
        /* as on the cabinet, the scores stay on top of the attract screen */
        render_header(fb, 0, state->hi_score);
        render_attract(fb, state->attract_frames);
        break;
    }
}
