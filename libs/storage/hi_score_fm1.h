/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The best score in the FM-1's flash, like @arcade/storage keeps it in the browser. Include from
 * main.c, after platform.c, with a STORE_MAGIC of the game's own defined before platform.c: the
 * platform then takes another firmware's settings for nothing saved.
 */
#ifndef HI_SCORE_FM1_H
#define HI_SCORE_FM1_H

/** What is saved: a version, so later fields can be added, and the score. */
typedef struct {
    uint32_t version;
    int32_t hi_score;
} HiScoreRecord;

#define HI_SCORE_VERSION 1u

/** The saved best score, or 0 when nothing (valid) is saved. */
static int32_t hi_score_load(void)
{
    HiScoreRecord record = {0u, 0};
    if (settings_load(&record, sizeof record) != 0 || record.version != HI_SCORE_VERSION ||
        record.hi_score < 0)
        return 0;
    return record.hi_score;
}

/**
 * Saves the best score. Erasing the flash holds the interrupts off for about 50 ms, so call it
 * when nothing moves and nothing sounds (after a game), and never in a loop: a sector wears out
 * after about 100,000 erases.
 */
static void hi_score_save(int32_t hi_score)
{
    const HiScoreRecord record = {HI_SCORE_VERSION, hi_score};
    (void)settings_save(&record, sizeof record);
}

#endif
