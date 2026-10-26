/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The best score of every game, as the firmware keeps them in the flash: one record per game,
 * marked with the game's four letters (Cartridge.record_tag), so the order of the menu can
 * change and a new game can come without losing the scores already saved.
 */
#ifndef RECORDS_H
#define RECORDS_H

#include <stdint.h>

/** At most this many games keep a score. */
#define RECORDS_MAX 8u

/** The version of what is saved: 1 was the Space Invaders firmware's single score. */
#define RECORDS_VERSION 2u

/** One game's best score. */
typedef struct {
    uint32_t tag;
    int32_t score;
} Record;

/** Every game's best score, as saved. */
typedef struct {
    uint32_t version;
    uint32_t count;
    Record list[RECORDS_MAX];
} Records;

/** No score saved. */
Records records_empty(void);

/** Whether `records` read from the flash make sense (version, count, no negative score). */
int records_valid(const Records *records);

/** The best score of the game marked `tag`, 0 when it has none. */
int32_t records_get(const Records *records, uint32_t tag);

/**
 * The records with `score` as the best of `tag`: replaced, or added (when there is room).
 */
Records records_set(const Records *records, uint32_t tag, int32_t score);

/** Whether two sets of records hold the same scores, in the same order. */
int records_equal(const Records *a, const Records *b);

#endif
