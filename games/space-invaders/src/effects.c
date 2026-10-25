/* SPDX-License-Identifier: GPL-3.0-only */
#include "effects.h"

#include "sprites.h"

/** How many frames each kind of effect stays on screen. */
static const uint8_t EFFECT_FRAMES[] = {16, 16, 16, 60};

/** Division rounding down, like Math.floor in the web version, also for negative numbers. */
static int32_t floor_div(int32_t a, int32_t b)
{
    const int32_t q = a / b;
    return (a % b != 0 && (a < 0) != (b < 0)) ? q - 1 : q;
}

uint32_t effect_frames(EffectKind kind)
{
    return EFFECT_FRAMES[kind];
}

Effects effects_none(void)
{
    Effects effects;
    effects.count = 0;
    return effects;
}

Effect effect_create(EffectKind kind, int32_t x, int32_t y, int32_t points)
{
    Effect effect = {(uint8_t)kind, EFFECT_FRAMES[kind], (int16_t)x, (int16_t)y, (int16_t)points};
    return effect;
}

Effect effect_explode_alien(const Alien *alien)
{
    const int32_t offset = floor_div(alien_width(alien) - EXPLOSION_SPRITE.width, 2);
    return effect_create(EFFECT_ALIEN, alien->x + offset, alien->y, NO_POINTS);
}

Effects effects_add(const Effects *effects, Effect effect)
{
    Effects next = *effects;
    if (next.count < MAX_EFFECTS)
        next.list[next.count++] = effect;
    return next;
}

Effects effects_step(const Effects *effects)
{
    Effects next = effects_none();
    for (uint32_t i = 0; i < effects->count; i++) {
        Effect effect = effects->list[i];
        if (effect.frames_left > 1u) {
            effect.frames_left--;
            next.list[next.count++] = effect;
        }
    }
    return next;
}
