/* SPDX-License-Identifier: GPL-3.0-only */
#include "fm1_keys.h"
#include "keymap.h"
#include "test/check.h"

static const KeyMap MAP = {
    .left = FM1_KEY_BIT(FM1_KEY_OP1),
    .right = FM1_KEY_BIT(FM1_KEY_GLO),
    .fire = FM1_KEY_BIT(FM1_KEY_OP4) | FM1_KEY_BIT(FM1_KEY_OP5),
};

static void no_keys_no_actions(void)
{
    CHECK_EQ(keymap_actions(&MAP, 0u), 0u);
}

static void each_key_asks_for_its_action(void)
{
    CHECK_EQ(keymap_actions(&MAP, FM1_KEY_BIT(FM1_KEY_OP1)), ACTION_LEFT);
    CHECK_EQ(keymap_actions(&MAP, FM1_KEY_BIT(FM1_KEY_GLO)), ACTION_RIGHT);
    CHECK_EQ(keymap_actions(&MAP, FM1_KEY_BIT(FM1_KEY_OP5)), ACTION_FIRE);
}

static void left_and_right_cancel_out_but_fire_stays(void)
{
    const uint32_t both = FM1_KEY_BIT(FM1_KEY_OP1) | FM1_KEY_BIT(FM1_KEY_GLO);
    CHECK_EQ(keymap_actions(&MAP, both), 0u);
    CHECK_EQ(keymap_actions(&MAP, both | FM1_KEY_BIT(FM1_KEY_OP4)), ACTION_FIRE);
}

static void keys_outside_the_map_are_ignored(void)
{
    CHECK_EQ(keymap_actions(&MAP, FM1_KEY_BIT(0) | FM1_KEY_BIT(FM1_KEY_POLY)), 0u);
}

static void the_map_lists_all_its_keys(void)
{
    CHECK_EQ(keymap_keys(&MAP), MAP.left | MAP.right | MAP.fire);
}

int main(void)
{
    RUN(no_keys_no_actions);
    RUN(each_key_asks_for_its_action);
    RUN(left_and_right_cancel_out_but_fire_stays);
    RUN(keys_outside_the_map_are_ignored);
    RUN(the_map_lists_all_its_keys);
    return check_report();
}
