/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The FM-1's 27 note keys by position, as bits of `fm1_in.notes` (bit 0 = F3, bit 26 = G5).
 *
 * The black keys carry a second label printed on the panel (OP1..OP6, PIT, GLO, MONO, POLY): the
 * official firmware uses them on its EDIT page. The games use those names, since they are what a
 * player reads on the keys. Source: `edit_keys` in app/screen.c of m-vave/fm1_open.
 */
#ifndef FM1_KEYS_H
#define FM1_KEYS_H

/** The bit of key `k` in `fm1_in.notes`. */
#define FM1_KEY_BIT(k) (1u << (k))

/** The labelled black keys, from left to right. */
enum {
    FM1_KEY_OP1 = 1,   /**< F#3 */
    FM1_KEY_OP2 = 3,   /**< G#3 */
    FM1_KEY_OP3 = 5,   /**< A#3 */
    FM1_KEY_OP4 = 8,   /**< C#4 */
    FM1_KEY_OP5 = 10,  /**< D#4 */
    FM1_KEY_OP6 = 13,  /**< F#4 */
    FM1_KEY_PIT = 15,  /**< G#4 */
    FM1_KEY_GLO = 17,  /**< A#4 */
    FM1_KEY_MONO = 20, /**< C#5 */
    FM1_KEY_POLY = 22, /**< D#5 */
};

/** How many note keys the FM-1 has. */
#define FM1_KEY_COUNT 27u

#endif
