/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_tracks.h"

int32_t envelope_volume(const TrackHeader *header, int32_t tick)
{
    if (header->envelope == 1 && tick < 6)
        return tick * 2;
    if (header->envelope >= 2 && tick < 6)
        return ~tick & 0x0f;
    if (header->hold == 0 || tick < header->hold)
        return 0x0a;
    int32_t fading = tick - header->hold;
    return fading >= 0x0a ? 0 : 0x0a - fading;
}

/** Whether `index` is one of the voices. */
static int is_voice(int32_t index)
{
    return index >= 0 && index < SOUND_VOICES;
}

int play_track(Work *work, int32_t effect, int32_t track, int32_t voice)
{
    if (track < 0 || track >= TRACK_COUNT)
        return 0;
    Tracks *tracks = &work->sound->tracks;
    int32_t tick = (tracks->ticks[track] + 1) & 0xff;
    TrackHeader header = track_header(track);
    int32_t address = header.notes + tracks->notes[track];
    int32_t note = sound_byte(address);
    tracks->ticks[track] = (uint8_t)tick;
    if (note == NOTE_END) {
        /* `l_06B2`: a finished track keeps its voice silent. */
        if (is_voice(voice))
            work->mix.volumes[voice] = 0;
        return 1;
    }
    if (is_voice(voice)) {
        work->mix.frequencies[voice] = note_pitch(note) * 16;
        work->mix.volumes[voice] = note == NOTE_REST ? 0 : envelope_volume(&header, tick);
        work->mix.waveforms[voice] = header.waveform;
    }
    /* The length of a note is its beats times the tempo, in one byte (`l_0661`). */
    int32_t length = (tempo(effect) * sound_byte(address + 1)) & 0xff;
    if (tick == length) {
        tracks->ticks[track] = 0;
        tracks->notes[track] = (uint8_t)(tracks->notes[track] + 2);
    }
    return 0;
}
