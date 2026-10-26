/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_commands.h"

#include "combat.h"
#include "sound_names.h"
#include "sound_state.h"

/** The effect each sound starts, in the order of the SOUND_* bits; the explosion starts none. */
static const uint8_t EFFECT_OF[SOUND_NAME_COUNT] = {
    EFFECT_SHOT,
    EFFECT_BEE_HIT,
    EFFECT_BUTTERFLY_HIT,
    EFFECT_BOSS_HIT,
    EFFECT_BOSS_DAMAGED,
    EFFECT_CAPTURED_FIGHTER_HIT,
    0xff, /* fighterExplosion */
    EFFECT_DIVE,
    EFFECT_START_THEME,
    EFFECT_CHALLENGE_START,
    EFFECT_BADGE,
    EFFECT_PULSE,
    EFFECT_EXTRA_LIFE,
    EFFECT_CHALLENGE_END,
    EFFECT_PERFECT,
    EFFECT_BONUS_BEE,
};

/** Adds a write to the frame's commands. */
static void add_write(SoundCommands *commands, uint8_t effect, int on)
{
    SoundWrite *w = &commands->writes[commands->count++];
    w->effect = effect;
    w->value = on ? 1 : 0;
}

/**
 * `l_226A`, `l_236D`: the beam hums while it opens, holds and closes; the main processor stops it
 * while a caught fighter waits for the next step of the beam, and while a shot Boss closes it.
 */
static int beam_hums(const Capture *capture)
{
    return (capture->tasks & CAPTURE_BEAM) && capture->step != 0 && (capture->step & 0x80) == 0 &&
           !(capture->step == 0x40 && (capture->tasks & CAPTURE_PULL));
}

/** `l_236D`-`l_22E3`: from the moment the fighter is caught to the end of the beam. */
static int fighter_pulled(const Capture *capture)
{
    return (capture->tasks & CAPTURE_BEAM) &&
           ((capture->tasks & CAPTURE_PULL) || capture->connected);
}

/** `l_19C7`-`l_1A6A`: the captured fighter's tune, from the timer at 3 until it is home. */
static int captured_tune(const PlayingState *state)
{
    return (state->capture.tasks & CAPTURE_CARRY) && !state->capture.show_text &&
           state->timers.t[1] <= 3;
}

/**
 * `gctl_supv_stage`, `f_0857`: the pulsing sound stops when the stage is cleared and when the last
 * aliens bomb without pause; only the next breathing starts it again.
 */
static int pulse_stops(const PlayingState *state)
{
    return (state->alive == 0 && !state->launcher.running) || is_continuous(state);
}

void no_sound_commands(SoundCommands *commands)
{
    commands->count = 0;
    commands->explosion = 0;
    commands->sign = 1;
}

int32_t formation_sign(uint8_t direction)
{
    uint8_t previous = direction == 0xa0 ? 0x1f : direction == 0 ? 0x81 : direction;
    return (previous & 0x80) ? 0xff : 1;
}

void sound_commands(const PlayingState *state, SoundCommands *commands)
{
    no_sound_commands(commands);
    for (int32_t i = 0; i < SOUND_NAME_COUNT; i++)
        if ((state->sounds & (1u << i)) && EFFECT_OF[i] != 0xff)
            add_write(commands, EFFECT_OF[i], 1);
    add_write(commands, EFFECT_TRACTOR_BEAM, beam_hums(&state->capture));
    add_write(commands, EFFECT_BEAM_CAPTURE, fighter_pulled(&state->capture));
    add_write(commands, EFFECT_FIGHTER_CAPTURED, captured_tune(state));
    add_write(commands, EFFECT_RESCUE, state->capture.tasks & CAPTURE_RESCUE);
    if (pulse_stops(state))
        add_write(commands, EFFECT_PULSE, 0);
    commands->explosion = (state->sounds & SOUND_FIGHTER_EXPLOSION) != 0;
    commands->sign = formation_sign(state->formation.direction);
}
