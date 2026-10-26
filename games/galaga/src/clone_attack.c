/* SPDX-License-Identifier: GPL-3.0-only */
#include "clone_attack.h"

#include "attacks.h"
#include "combat.h"
#include "rom.h"
#include "table.h"

/** The three programs of the bonus bee, by its flashing palette 4, 5, 6 (`d_1B5F`). */
static const uint16_t CLONE_PROGRAMS[3] = {0x04ea, 0x0473, 0x04ab};

/** Where the timer starts, and where it goes back when the fighter cannot fire. */
#define FLASH_START 0xc0
#define FLASH_AGAIN 0xe0

/** The task ends for this stage (`l_1B54`). */
static void stop(PlayingState *state)
{
    set_tasks(state, 0, TASK_CLONES);
}

/** The first resting alien of `count` from `first`, or -1. */
static int32_t first_resting(const Objects *objects, int32_t first, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
        if (object_at(objects, first + i * 2)->disposition == DISPOSITION_HOME)
            return first + i * 2;
    return -1;
}

/** `l_1AAB`: the first resting bee, else butterfly, becomes the bonus bee. */
static void choose(PlayingState *state)
{
    int32_t id = first_resting(&state->objects, 0x08, 20);
    if (id < 0)
        id = first_resting(&state->objects, 0x40, 16);
    if (id < 0)
        return;
    state->attacks.bonus_bee = (uint8_t)id;
    state->attacks.bonus_bee_timer = FLASH_START;
    state->attacks.bonus_bee_look = object_at(&state->objects, id)->color;
    state->attacks.bonus_bee_color = (uint8_t)((state->stage / 4) % 3 + 4);
    play(state, SOUND_BONUS_BEE);
}

/** `l_1B00`: the bonus bee dives with the look of a special enemy, the bonus is armed. */
static void launch_clones(PlayingState *state)
{
    Attacks *attacks = &state->attacks;
    GameObject *bee = object_ref(&state->objects, attacks->bonus_bee);
    if (bee->disposition != DISPOSITION_HOME || (bee->hit & 0x80) != 0) {
        stop(state);
        return;
    }
    int32_t kind = attacks->bonus_bee_color - 4;
    attacks->bonus_bee_look = (uint8_t)((bee->code & 0xf8) | (attacks->bonus_bee_look & 0x07));
    bee->code = (uint8_t)(kind * 8 + 0x56);
    state->triple_left = 3;
    state->triple_bonus.hundreds = (uint8_t)TABLE(CLONE_BONUS, kind * 2);
    state->triple_bonus.code = (uint8_t)TABLE(CLONE_BONUS, kind * 2 + 1);
    AttackWorld world = {attacks, &state->objects, &state->slots, &state->capture, 0};
    QueuedLaunch dive = {attacks->bonus_bee, (uint8_t)((attacks->bonus_bee & 2) != 0),
                         kind >= 0 && kind < 3 ? CLONE_PROGRAMS[kind] : PROGRAM_BEE};
    launch_diver(&world, &dive, state->enemies_enabled ? current_settings(state).bomb_bits : 0);
    stop(state);
}

/** `l_1AD5`: the bonus bee flashes until the timer runs out, then dives. */
static void flash(PlayingState *state)
{
    Attacks *attacks = &state->attacks;
    int32_t timer = (attacks->bonus_bee_timer + 1) & 0xff;
    if (timer == 0) {
        if (task_on(state, TASK_FIRE))
            launch_clones(state);
        else
            attacks->bonus_bee_timer = FLASH_AGAIN;
        return;
    }
    GameObject *bee = object_ref(&state->objects, attacks->bonus_bee);
    if (bee->disposition != DISPOSITION_HOME) {
        stop(state);
        return;
    }
    attacks->bonus_bee_timer = timer;
    bee->color =
        (uint8_t)((timer & 0x10) != 0 ? attacks->bonus_bee_color : attacks->bonus_bee_look & 0x07);
}

void clone_attack(PlayingState *state)
{
    if (!task_on(state, TASK_CLONES) || state->alive >= state->params.clone_attack)
        return;
    if (state->attacks.bonus_bee_timer == 0)
        choose(state);
    else
        flash(state);
}
