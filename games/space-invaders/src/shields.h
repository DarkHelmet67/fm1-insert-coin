/* SPDX-License-Identifier: GPL-3.0-only */
/** @file The four shields, which crumble at every hit (insert-coin, shields.ts). */
#ifndef SHIELDS_H
#define SHIELDS_H

#include "sprite/bitmap.h"

/** Top edge of the shields, left edge of the first one, distance between two of them. */
#define SHIELD_Y 192
#define FIRST_SHIELD_X 32
#define SHIELD_SPACING 45
#define SHIELD_COUNT 4

/** One shield: its position and its current, possibly damaged, shape. */
typedef struct {
    int16_t x;
    int16_t y;
    Bitmap bitmap;
} Shield;

/** The four shields. */
typedef struct {
    Shield list[SHIELD_COUNT];
} Shields;

/** The four intact shields of a new round. */
Shields shields_create(void);

/** Whether any shield is touched by `object`, with pixel-perfect precision. */
int shields_hit(const Shields *shields, const Placed *object);

/** Carves `brush` out of every shield it touches. */
Shields shields_damage(const Shields *shields, const Placed *brush);

#endif
