/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `ds_99B9_star_ctrl` and `f_1D76` (star-control.ts): what decides the speed of the stars. They
 * stand still until the fighter is on screen, then speed up to the speed of the stage; they run
 * backwards fast while the fighter is being captured.
 */
#ifndef GALAGA_STAR_CONTROL_H
#define GALAGA_STAR_CONTROL_H

#include <stdint.h>

#include "cartridge/private.h"

/** The star control bytes. */
typedef struct {
    uint8_t ship_on_screen; /**< the fighter is on screen (99B9) */
    uint8_t backwards;      /**< the stars run backwards (99BA) */
    uint8_t target;         /**< target speed, 1/64 pixel per frame (99BB) */
    uint8_t current;        /**< current speed (99BC) */
    uint8_t remainder;      /**< the fraction carried over (99BD) */
} StarControl;

/** Stars standing still. */
GAME_PRIVATE StarControl stars_stopped(void);

/**
 * `f_1D76`: one frame of star control; returns the speed register of the chip (stars.h). The
 * speed ramps up by 1/64 pixel per frame to the target; the pixels to move this frame are the
 * whole part of the accumulated speed, and the speed register is 7 minus that.
 */
GAME_PRIVATE int32_t control_stars(StarControl *control);

#endif
