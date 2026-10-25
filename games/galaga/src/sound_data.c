/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_data.h"

#include "rom.h"

/** `d_06A9_ndat`: the pitch of the 12 notes of the top octave, then a rest (0). */
#define NOTE_TABLE 0x06d0

/** `d_0703_snd_parms`: 3 bytes for each effect: first track, tracks, first voice. */
#define EFFECT_TABLE 0x072a

/** `d_0748_p_snd_fx`: the address of each track. */
#define TRACK_TABLE 0x076f

/** `d_07A6`: the length of a beat of each effect, in sound ticks (1/120 s). */
#define TEMPO_TABLE 0x07cd

int32_t sound_byte(int32_t address)
{
    int32_t offset = address - (int32_t)SOUND_DATA_ADDRESS;
    return offset >= 0 && offset < (int32_t)SOUND_DATA_SIZE ? SOUND_DATA[offset] : 0xff;
}

/** A 16-bit word, low byte first. */
static int32_t sound_word(int32_t address)
{
    return sound_byte(address) | (sound_byte(address + 1) << 8);
}

EffectTracks effect_tracks(int32_t effect)
{
    int32_t address = EFFECT_TABLE + effect * 3;
    EffectTracks tracks;
    tracks.first = sound_byte(address);
    tracks.count = sound_byte(address + 1);
    tracks.voice = sound_byte(address + 2);
    return tracks;
}

TrackHeader track_header(int32_t track)
{
    int32_t address = sound_word(TRACK_TABLE + track * 2);
    TrackHeader header;
    header.envelope = sound_byte(address);
    header.hold = sound_byte(address + 1);
    header.waveform = sound_byte(address + 2);
    header.notes = address + 3;
    return header;
}

int32_t tempo(int32_t effect)
{
    return sound_byte(TEMPO_TABLE + effect);
}

int32_t note_pitch(int32_t note)
{
    return sound_word(NOTE_TABLE + (note & 0x0f) * 2) >> (note >> 4);
}

PulseSegment pulse_segment(int32_t table, int32_t segment)
{
    int32_t address = table + segment * 2;
    PulseSegment pulse;
    pulse.step = sound_word(address);
    pulse.start = sound_word(address + 0x20);
    return pulse;
}
