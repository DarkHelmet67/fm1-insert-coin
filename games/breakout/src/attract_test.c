/* SPDX-License-Identifier: GPL-3.0-only */
#include "attract.h"
#include "test/check.h"

static void the_attract_mode_breaks_nothing(void)
{
    Round round = round_new();
    for (uint32_t clock = 0; clock < 3000; clock++)
        round = attract_step(&round, clock);
    CHECK_EQ(round.score, 0);
    CHECK_EQ(round.ball, 1);
    CHECK_EQ(round.wall.standing[0], 1);
    CHECK_EQ(round.play.phase, PHASE_IN_PLAY);
}

int main(void)
{
    RUN(the_attract_mode_breaks_nothing);
    return check_report();
}
