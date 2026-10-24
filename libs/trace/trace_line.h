/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * One line of a reference trace (tools/traces/FORMAT.md): the frame, the controls given to the
 * game in that frame, the screen after it and the hash of the whole state.
 *
 *     1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18
 *
 * The numbers after the hash are for people reading the file; the parser ignores them.
 */
#ifndef TRACE_LINE_H
#define TRACE_LINE_H

#include <stdint.h>

/** What a trace line says. */
typedef struct {
    uint32_t frame;
    int32_t direction; /**< -1 left, 0 still, 1 right */
    int fire;          /**< fire pressed in this frame */
    int coin;          /**< a coin inserted in this frame */
    char screen;       /**< 'A' attract, 'P' playing, 'G' game over */
    uint32_t hash;     /**< FNV-1a of the state string */
} TraceLine;

/**
 * Reads one line of a trace.
 * @param line the text, without or with its newline
 * @param out  filled when the line is a frame
 * @returns 1 for a frame, 0 for a comment, an empty line or anything malformed
 */
int trace_parse_line(const char *line, TraceLine *out);

#endif
