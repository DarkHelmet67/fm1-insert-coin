/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"
#include "console.h"
#include "menu.h"
#include "screen/draw.h"
#include "test/check.h"

/** A pretend game: it counts its frames, scores 10 a frame while playing, PLAY starts it. */
typedef struct {
    uint32_t frames;
    int32_t score;
    int32_t hi_score;
    int playing;
    uint32_t sounds;
} Toy;

static void toy_start(void *state, int32_t hi_score)
{
    const Toy toy = {0u, 0, hi_score, 0, 0u};
    *(Toy *)state = toy;
}

static void toy_step(void *state, const Panel *panel)
{
    Toy *toy = state;
    toy->frames++;
    toy->sounds = 0;
    if (!toy->playing && panel->button_presses & FM1_BUTTON_BIT(FM1_BUTTON_PLAY)) {
        toy->playing = 1;
        toy->sounds = 1u;
    }
    if (toy->playing && panel->notes) /* any key ends the game */
        toy->playing = 0;
    else if (toy->playing)
        toy->score += 10;
    if (toy->score > toy->hi_score)
        toy->hi_score = toy->score;
}

static void toy_render(const void *state, Framebuffer *fb)
{
    (void)state;
    fb_clear(fb, 1);
}

static const uint16_t TOY_COLORS[FB_COLORS] = {0x0000, 0x1234};

static const uint16_t *toy_colors(const void *state)
{
    (void)state;
    return TOY_COLORS;
}

static uint32_t toy_sounds(const void *state)
{
    return ((const Toy *)state)->sounds;
}

static int32_t toy_hi_score(const void *state)
{
    return ((const Toy *)state)->hi_score;
}

static int toy_playing(const void *state)
{
    return ((const Toy *)state)->playing;
}

static uint32_t toy_keys(void)
{
    return 0x10u;
}

static ScreenMap toy_screen_map(void)
{
    return screen_map_centered(64, 32);
}

/** The second toy has a sound chip, playing one note. */
static const WsgFrame TOY_VOICES = {{{{1000u, 15u, 0u}}, {{1000u, 15u, 0u}}}};
static const uint8_t TOY_PROM[WSG_PROM_SIZE] = {0};

static const WsgFrame *toy_voices(const void *state)
{
    (void)state;
    return &TOY_VOICES;
}

static const Cartridge TOY_A = {
    .name = "TOY A",
    .record_tag = 0x41u,
    .state_size = sizeof(Toy),
    .width = 64,
    .height = 32,
    .screen_map = toy_screen_map,
    .start = toy_start,
    .step = toy_step,
    .render = toy_render,
    .colors = toy_colors,
    .sounds = toy_sounds,
    .hi_score = toy_hi_score,
    .playing = toy_playing,
    .keys = toy_keys,
};
static const Cartridge TOY_B = {
    .name = "TOY B",
    .record_tag = 0x42u,
    .state_size = sizeof(Toy),
    .width = 64,
    .height = 32,
    .screen_map = toy_screen_map,
    .start = toy_start,
    .step = toy_step,
    .render = toy_render,
    .colors = toy_colors,
    .sounds = toy_sounds,
    .voices = toy_voices,
    .wsg_prom = TOY_PROM,
    .hi_score = toy_hi_score,
    .playing = toy_playing,
    .keys = toy_keys,
};
static const Cartridge *const TOYS[] = {&TOY_A, &TOY_B};

static Console console;
static Framebuffer screen;
static Framebuffer spare;
static const Panel NOTHING = {0u, 0u, 0u, 0u, {0}};

static const Toy *toy(void)
{
    return (const Toy *)console.state.bytes;
}

static Panel play(void)
{
    Panel panel = NOTHING;
    panel.buttons = panel.button_presses = FM1_BUTTON_BIT(FM1_BUTTON_PLAY);
    return panel;
}

static void start(void)
{
    Records records = records_empty();
    records = records_set(&records, 0x42u, 700);
    console_start(&console, TOYS, 2, &records);
}

static void the_menu_runs_the_chosen_game_silently(void)
{
    start();
    CHECK(console_in_menu(&console));
    console_step(&console, &NOTHING);
    console_step(&console, &NOTHING);
    CHECK_EQ(toy()->frames, 2);
    CHECK_EQ(console.sounds, 0);
    CHECK_EQ(console_width(&console), MENU_SIZE);
    CHECK(console_waiting(&console));
}

static void choosing_another_game_switches_it_on_with_its_record(void)
{
    Panel select = NOTHING;
    select.turns[FM1_ENCODER_SELECT] = 1;
    start();
    console_step(&console, &select);
    CHECK(console_cartridge(&console) == &TOY_B);
    CHECK_EQ(toy()->frames, 0);
    CHECK_EQ(toy()->hi_score, 700);
}

static void play_enters_the_game_without_starting_it(void)
{
    const Panel press = play();
    start();
    console_step(&console, &press);
    CHECK(!console_in_menu(&console));
    CHECK(!toy()->playing); /* the press that left the menu is not the game's */
    CHECK_EQ(console_width(&console), 64);
    console_step(&console, &press);
    CHECK(toy()->playing);
    CHECK_EQ(console.sounds, 1);
    CHECK_EQ(console_keys(&console), 0x10u);
}

static void a_record_is_kept_when_the_game_is_over(void)
{
    const Panel press = play();
    Panel key = NOTHING;
    key.notes = 1u;
    start();
    console_step(&console, &press);
    console_step(&console, &press);
    for (int i = 0; i < 5; i++)
        console_step(&console, &NOTHING);
    CHECK_EQ(records_get(&console.records, 0x41u), 0); /* still playing */
    console_step(&console, &key);
    CHECK_EQ(records_get(&console.records, 0x41u), 60); /* 10 from the frame it started */
}

static void leaving_a_game_restarts_it_behind_the_menu(void)
{
    const Panel press = play();
    Panel held = NOTHING;
    held.buttons = FM1_BUTTON_BIT(FM1_BUTTON_PLAY);
    start();
    console_step(&console, &press);
    console_step(&console, &press);
    for (uint32_t i = 1; i < LEAVE_FRAMES; i++) /* the press was the first frame held */
        console_step(&console, &held);
    CHECK(console_in_menu(&console));
    CHECK(!toy()->playing);
    CHECK_EQ(toy()->frames, 0);
    CHECK(records_get(&console.records, 0x41u) > 0); /* the score of the game left halfway */
}

static void the_menu_shows_the_game_and_the_menus_colours(void)
{
    uint16_t colors[FB_COLORS];
    start();
    console_colors(&console, colors);
    CHECK_EQ(colors[1], 0x1234);
    CHECK_EQ(colors[MENU_COLOR_TITLE], MENU_COLORS[MENU_COLOR_TITLE]);
    fb_init(&screen, MENU_SIZE, MENU_SIZE);
    console_render(&console, &screen, &spare);
    CHECK_EQ(fb_get(&screen, 120, 88), 1); /* the middle of the picture: the toy's colour */
}

static void the_sound_chip_plays_only_in_its_game(void)
{
    Panel select = NOTHING;
    select.turns[FM1_ENCODER_SELECT] = 1;
    const Panel press = play();
    start();
    CHECK(console_voices(&console) == 0); /* TOY A has no chip */
    console_step(&console, &select);
    CHECK(console_voices(&console) == 0); /* TOY B, but behind the menu */
    console_step(&console, &press);
    CHECK(console_voices(&console) == &TOY_VOICES);
}

int main(void)
{
    RUN(the_menu_runs_the_chosen_game_silently);
    RUN(choosing_another_game_switches_it_on_with_its_record);
    RUN(play_enters_the_game_without_starting_it);
    RUN(a_record_is_kept_when_the_game_is_over);
    RUN(leaving_a_game_restarts_it_behind_the_menu);
    RUN(the_menu_shows_the_game_and_the_menus_colours);
    RUN(the_sound_chip_plays_only_in_its_game);
    return check_report();
}
