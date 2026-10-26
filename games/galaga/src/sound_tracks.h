/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * `c_0550` (at $0577 in this ROM, sound-tracks.ts): one tick of one track of an effect. The
 * track plays its notes one after the other on one voice: each note is a pitch and a length in
 * beats, and its volume follows the envelope written in the track's header.
 */
#ifndef GALAGA_SOUND_TRACKS_H
#define GALAGA_SOUND_TRACKS_H

#include <stdint.h>

#include "cartridge/private.h"
#include "sound_data.h"
#include "sound_state.h"

/**
 * `l_0603`-`l_0654`: the volume of a note `tick` ticks after it started. Envelope 0 holds volume
 * 10, then fades by one a tick; 1 rises from 2 to 10 first; 2 and above start at 14 and fall to
 * 10 first. The voices only read the low 4 bits.
 */
GAME_PRIVATE int32_t envelope_volume(const TrackHeader *header, int32_t tick);

/**
 * One tick of `track` of `effect`, played on `voice`. Returns 1 when the track has run out of
 * notes (`b_9A70[8]`).
 */
GAME_PRIVATE int play_track(Work *work, int32_t effect, int32_t track, int32_t voice);

#endif
