/* SPDX-License-Identifier: GPL-3.0-only */
#include "records.h"

Records records_empty(void)
{
    Records records = {RECORDS_VERSION, 0u, {{0u, 0}}};
    return records;
}

int records_valid(const Records *records)
{
    if (records->version != RECORDS_VERSION || records->count > RECORDS_MAX)
        return 0;
    for (uint32_t i = 0; i < records->count; i++)
        if (records->list[i].score < 0)
            return 0;
    return 1;
}

/** Where `tag` is in the list, or `count` when it is not there. */
static uint32_t records_find(const Records *records, uint32_t tag)
{
    uint32_t i = 0;
    while (i < records->count && records->list[i].tag != tag)
        i++;
    return i;
}

int32_t records_get(const Records *records, uint32_t tag)
{
    const uint32_t i = records_find(records, tag);
    return i < records->count ? records->list[i].score : 0;
}

Records records_set(const Records *records, uint32_t tag, int32_t score)
{
    Records next = *records;
    const uint32_t i = records_find(records, tag);
    if (i == RECORDS_MAX)
        return next;
    next.list[i].tag = tag;
    next.list[i].score = score;
    if (i == records->count)
        next.count++;
    return next;
}

int records_equal(const Records *a, const Records *b)
{
    if (a->version != b->version || a->count != b->count)
        return 0;
    for (uint32_t i = 0; i < a->count; i++)
        if (a->list[i].tag != b->list[i].tag || a->list[i].score != b->list[i].score)
            return 0;
    return 1;
}
