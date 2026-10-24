/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Short-lived things drawn after a hit: explosions, splashes and the UFO's points
 * (insert-coin, effects.ts).
 */
#ifndef EFFECTS_H
#define EFFECTS_H

#include <stdint.h>

#include "aliens.h"

/** The kinds of effect. */
typedef enum { EFFECT_ALIEN, EFFECT_SHOT, EFFECT_BOMB, EFFECT_UFO } EffectKind;

/** More effects than can ever be on screen together. */
#define MAX_EFFECTS 24

/** No points shown by the effect. */
#define NO_POINTS (-1)

/** One effect on screen. */
typedef struct {
    uint8_t kind;        /**< an EffectKind */
    uint8_t frames_left; /**< frames before it disappears */
    int16_t x;
    int16_t y;
    int16_t points; /**< points shown by a UFO effect, or NO_POINTS */
} Effect;

/** The effects on screen, oldest first. */
typedef struct {
    Effect list[MAX_EFFECTS];
    uint8_t count;
} Effects;

/** No effects. */
Effects effects_none(void);

/** A new effect of `kind` at (x, y), lasting as long as its kind does. */
Effect effect_create(EffectKind kind, int32_t x, int32_t y, int32_t points);

/** An explosion centred on the invader that was hit. */
Effect effect_explode_alien(const Alien *alien);

/** The effects with one more at the end (dropped when the list is full). */
Effects effects_add(const Effects *effects, Effect effect);

/** Ages the effects by one frame and removes the ones that have finished. */
Effects effects_step(const Effects *effects);

#endif
