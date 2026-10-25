/* SPDX-License-Identifier: GPL-3.0-only */
#include "extra_lives.h"

#include "config.h"

int32_t next_bonus_after(int32_t bonus)
{
    return bonus < EXTRA_LIFE_EVERY ? EXTRA_LIFE_EVERY : bonus + EXTRA_LIFE_EVERY;
}

void add_points(PlayingState *state, int32_t points)
{
    if (points == 0)
        return;
    state->score += points;
    if (state->score / 10000 != state->next_bonus)
        return;
    state->reserve++;
    state->next_bonus = next_bonus_after(state->next_bonus);
    play(state, SOUND_EXTRA_LIFE);
}
