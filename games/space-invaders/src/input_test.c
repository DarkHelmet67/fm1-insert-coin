/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"
#include "input.h"
#include "test/check.h"

#define OP1 FM1_KEY_BIT(FM1_KEY_OP1)
#define OP4 FM1_KEY_BIT(FM1_KEY_OP4)
#define OP5 FM1_KEY_BIT(FM1_KEY_OP5)
#define GLO FM1_KEY_BIT(FM1_KEY_GLO)

static void moving_follows_the_keys_held(void)
{
    CHECK_EQ(input_controls(&CONTROLS, OP1, OP1, 0, COIN_BUTTON).direction, -1);
    CHECK_EQ(input_controls(&CONTROLS, GLO, 0, 0, COIN_BUTTON).direction, 1);
    CHECK_EQ(input_controls(&CONTROLS, OP1 | GLO, 0, 0, COIN_BUTTON).direction, 0);
}

static void fire_is_a_press_not_a_hold(void)
{
    CHECK(input_controls(&CONTROLS, OP4, OP4, 0, COIN_BUTTON).fire);
    CHECK(!input_controls(&CONTROLS, OP4, 0, 0, COIN_BUTTON).fire);
    CHECK(input_controls(&CONTROLS, OP4 | OP5, OP5, 0, COIN_BUTTON).fire);
}

static void a_tap_shorter_than_a_frame_still_fires(void)
{
    CHECK(input_controls(&CONTROLS, 0, OP5, 0, COIN_BUTTON).fire);
}

static void the_coin_is_a_press_of_its_button(void)
{
    CHECK(input_controls(&CONTROLS, 0, 0, 1u << COIN_BUTTON, COIN_BUTTON).coin);
    CHECK(!input_controls(&CONTROLS, 0, 0, 1u << COLOR_BUTTON, COIN_BUTTON).coin);
}

int main(void)
{
    RUN(moving_follows_the_keys_held);
    RUN(fire_is_a_press_not_a_hold);
    RUN(a_tap_shorter_than_a_frame_still_fires);
    RUN(the_coin_is_a_press_of_its_button);
    return check_report();
}
