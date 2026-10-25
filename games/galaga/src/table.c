/* SPDX-License-Identifier: GPL-3.0-only */
#include "table.h"

int32_t table_at(const uint8_t *table, uint32_t size, int32_t index, int32_t fallback)
{
    return index >= 0 && (uint32_t)index < size ? table[index] : fallback;
}
