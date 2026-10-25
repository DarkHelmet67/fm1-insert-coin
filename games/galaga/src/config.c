/* SPDX-License-Identifier: GPL-3.0-only */
#include "config.h"

#include "input/fm1_keys.h"

GAME_PRIVATE const KeyMap CONTROLS = {
    .left = FM1_KEY_BIT(FM1_KEY_OP1) | FM1_KEY_BIT(FM1_KEY_OP2) | FM1_KEY_BIT(FM1_KEY_OP3),
    .fire = FM1_KEY_BIT(FM1_KEY_OP4) | FM1_KEY_BIT(FM1_KEY_OP5),
    .right = FM1_KEY_BIT(FM1_KEY_OP6) | FM1_KEY_BIT(FM1_KEY_PIT) | FM1_KEY_BIT(FM1_KEY_GLO),
};

/** Rows 6k to 6k + 4 of the original. */
#define FIVE_OF_SIX(k)                                                                             \
    {                                                                                              \
        6u * (k), 6u * (k) + 4u                                                                    \
    }

GAME_PRIVATE const RowRange SHOWN_ROWS[SQUEEZE_RANGES] = {
    FIVE_OF_SIX(0),  FIVE_OF_SIX(1),  FIVE_OF_SIX(2),  FIVE_OF_SIX(3),  FIVE_OF_SIX(4),
    FIVE_OF_SIX(5),  FIVE_OF_SIX(6),  FIVE_OF_SIX(7),  FIVE_OF_SIX(8),  FIVE_OF_SIX(9),
    FIVE_OF_SIX(10), FIVE_OF_SIX(11), FIVE_OF_SIX(12), FIVE_OF_SIX(13), FIVE_OF_SIX(14),
    FIVE_OF_SIX(15), FIVE_OF_SIX(16), FIVE_OF_SIX(17), FIVE_OF_SIX(18), FIVE_OF_SIX(19),
    FIVE_OF_SIX(20), FIVE_OF_SIX(21), FIVE_OF_SIX(22), FIVE_OF_SIX(23), FIVE_OF_SIX(24),
    FIVE_OF_SIX(25), FIVE_OF_SIX(26), FIVE_OF_SIX(27), FIVE_OF_SIX(28), FIVE_OF_SIX(29),
    FIVE_OF_SIX(30), FIVE_OF_SIX(31), FIVE_OF_SIX(32), FIVE_OF_SIX(33), FIVE_OF_SIX(34),
    FIVE_OF_SIX(35), FIVE_OF_SIX(36), FIVE_OF_SIX(37), FIVE_OF_SIX(38), FIVE_OF_SIX(39),
    FIVE_OF_SIX(40), FIVE_OF_SIX(41), FIVE_OF_SIX(42), FIVE_OF_SIX(43), FIVE_OF_SIX(44),
    FIVE_OF_SIX(45), FIVE_OF_SIX(46), FIVE_OF_SIX(47),
};

/** The explosion: cutoff 1200 Hz, 1.1 s, volume 0.6, held 0.05 s, then a straight fade. */
static const Sound EXPLOSION[] = {
    {SOUND_NOISE, 0, 0, CURVE_LINEAR, 0, 0, 1200, 0, 1100, 50, 600},
};

GAME_PRIVATE const SoundEffect SOUND_EFFECTS[1] = {SOUND_EFFECT(EXPLOSION)};
