/* SPDX-License-Identifier: GPL-3.0-only */
#include "star_control.h"

#include "stars.h"

/** The speed register value that runs the stars backwards: 2 extra steps (`l_1D9B`). */
#define BACKWARDS_SPEED 2

StarControl stars_stopped(void)
{
    StarControl control = {0, 0, 0x40, 0, 0};
    return control;
}

int32_t control_stars(StarControl *control)
{
    if (!control->ship_on_screen) {
        control->current = 0;
        control->remainder = 0;
        return STARS_STILL;
    }
    if (control->backwards)
        return BACKWARDS_SPEED;
    if (control->current != control->target)
        control->current++;
    int32_t sum = control->current + control->remainder;
    int32_t pixels = (sum >> 6) & 3;
    control->remainder = (uint8_t)(sum & 0x3f);
    return (-pixels - 1) & 7;
}
