/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sounds the game starts at a moment (sound-names.ts), as the main processor sets the
 * trigger registers of the sound processor (`b_9AA0`): one bit each in a frame's set of sounds.
 * The sounds that last as long as something happens (the tractor beam, the captured fighter's
 * tune, the rescue) are read from the state instead.
 */
#ifndef GALAGA_SOUND_NAMES_H
#define GALAGA_SOUND_NAMES_H

/** One bit per sound, in the order of the web version's list. */
enum {
    SOUND_SHOT = 1 << 0,
    SOUND_BEE_HIT = 1 << 1,
    SOUND_BUTTERFLY_HIT = 1 << 2,
    SOUND_BOSS_HIT = 1 << 3,
    SOUND_BOSS_DAMAGED = 1 << 4,
    SOUND_CAPTURED_FIGHTER_HIT = 1 << 5, /**< a captured fighter, red, was shot */
    SOUND_FIGHTER_EXPLOSION = 1 << 6,    /**< the noise of the 54XX chip, not a melody */
    SOUND_DIVE = 1 << 7,
    SOUND_START_THEME = 1 << 8,
    SOUND_CHALLENGE_START = 1 << 9,
    SOUND_BADGE = 1 << 10,
    SOUND_PULSE = 1 << 11, /**< the formation starts breathing, with its pulsing sound */
    SOUND_EXTRA_LIFE = 1 << 12,
    SOUND_CHALLENGE_END = 1 << 13,
    SOUND_PERFECT = 1 << 14,
    SOUND_BONUS_BEE = 1 << 15,
};

/** Number of sounds. */
#define SOUND_NAME_COUNT 16

#endif
