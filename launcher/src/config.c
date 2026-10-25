/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"

const uint16_t MENU_COLORS[FB_COLORS] = {
    [MENU_COLOR_TEXT] = RGB565(255, 255, 255),
    [MENU_COLOR_TITLE] = RGB565(255, 214, 0),
    [MENU_COLOR_DIM] = RGB565(110, 110, 110),
    [MENU_COLOR_FRAME] = RGB565(0, 160, 255),
};
