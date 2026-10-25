/* SPDX-License-Identifier: GPL-3.0-only */
#include "effects.h"
#include "test/check.h"

static void it_centres_the_explosion_on_the_invader(void)
{
    const Alien squid = formation_alien(44, 1); /* 8 wide: the 13-wide burst starts 3 to the left */
    const Effect burst = effect_explode_alien(&squid);
    CHECK_EQ(burst.x, squid.x - 3);
    CHECK_EQ(burst.y, squid.y);
    const Alien octopus = formation_alien(0, 1);
    CHECK_EQ(effect_explode_alien(&octopus).x, octopus.x - 1);
}

static void it_keeps_the_points_of_the_mystery_ship(void)
{
    const Effect effect = effect_create(EFFECT_UFO, 10, 40, 300);
    CHECK_EQ(effect.points, 300);
    CHECK_EQ(effect.frames_left, 60);
}

static void it_disappears_after_its_frames_have_elapsed(void)
{
    const Effects none = effects_none();
    Effects effects = effects_add(&none, effect_create(EFFECT_SHOT, 1, 2, NO_POINTS));
    for (int i = 0; i < 15; i++)
        effects = effects_step(&effects);
    CHECK_EQ(effects.count, 1);
    effects = effects_step(&effects);
    CHECK_EQ(effects.count, 0);
}

int main(void)
{
    RUN(it_centres_the_explosion_on_the_invader);
    RUN(it_keeps_the_points_of_the_mystery_ship);
    RUN(it_disappears_after_its_frames_have_elapsed);
    return check_report();
}
