/* SPDX-License-Identifier: GPL-3.0-only */
#include "capture_state.h"

Capture no_capture(void)
{
    Capture capture = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return capture;
}

void set_beam_row(Capture *capture, int32_t row, int drawn)
{
    if (row < 1 || row > BEAM_ROWS)
        return;
    uint16_t bit = (uint16_t)(1u << (row - 1));
    capture->beam_rows = (uint16_t)(drawn ? capture->beam_rows | bit : capture->beam_rows & ~bit);
}

int beam_row(const Capture *capture, int32_t row)
{
    return row >= 1 && row <= BEAM_ROWS && (capture->beam_rows >> (row - 1)) & 1;
}

void set_capture_tasks(Capture *capture, int32_t on, int32_t off)
{
    capture->tasks = (uint8_t)((capture->tasks | on) & ~off);
}
