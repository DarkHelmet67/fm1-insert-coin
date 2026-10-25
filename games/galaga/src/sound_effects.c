/* SPDX-License-Identifier: GPL-3.0-only */
#include "sound_effects.h"

#include "sound_data.h"
#include "sound_tracks.h"

/** Track of the challenge results tune (`b_9A30[$1C]`): its three tracks start in turn. */
#define ROUND_TRACK 0x1c

/**
 * `l_0393`: the tracks of `effect`. The challenge results tune is a round: its second track
 * joins when the first has played a note, the third when the second has.
 */
static EffectTracks tracks_of(const Work *work, int32_t effect)
{
    EffectTracks tracks = effect_tracks(effect);
    if (effect != EFFECT_CHALLENGE_END)
        return tracks;
    const uint8_t *notes = work->sound->tracks.notes;
    if (notes[ROUND_TRACK] == 0)
        tracks.count = 1;
    else if (notes[ROUND_TRACK] == 1 || notes[ROUND_TRACK + 1] == 0)
        tracks.count = 2;
    return tracks;
}

/** `l_03B7`: the effect starts: its tracks go back to their first note. */
static void start(Work *work, int32_t effect)
{
    EffectTracks tracks = tracks_of(work, effect);
    for (int32_t i = tracks.first; i < tracks.first + tracks.count && i < TRACK_COUNT; i++) {
        work->sound->tracks.ticks[i] = 0;
        work->sound->tracks.notes[i] = 0;
    }
    work->sound->playing[effect] = 1;
}

/** `l_03D2`: one tick of every track of the effect, each on its voice; 1 when one ran out. */
static int play_tracks(Work *work, int32_t effect)
{
    EffectTracks tracks = tracks_of(work, effect);
    int ended = 0;
    for (int32_t i = 0; i < tracks.count; i++)
        ended |= play_track(work, effect, tracks.first + i, tracks.voice + i);
    return ended;
}

/** Plays the effect one tick; it is no longer playing when it has ended. */
static void play_until_ended(Work *work, int32_t effect)
{
    if (play_tracks(work, effect))
        work->sound->playing[effect] = 0;
}

void restarting(Work *work, int32_t effect)
{
    if (work->sound->triggers[effect] != 0) {
        work->sound->triggers[effect] = 0;
        start(work, effect);
        play_until_ended(work, effect);
    } else if (work->sound->playing[effect]) {
        play_until_ended(work, effect);
    }
}

/** The effect started if it is not playing yet. */
static void start_once(Work *work, int32_t effect)
{
    if (!work->sound->playing[effect])
        start(work, effect);
}

void held(Work *work, int32_t effect)
{
    if (work->sound->triggers[effect] == 0) {
        work->sound->playing[effect] = 0;
        return;
    }
    start_once(work, effect);
    play_until_ended(work, effect);
}

/** Whether "shot your ship!" keeps `effect` when it clears the others: coins, extra fighter. */
static int kept(int32_t effect)
{
    return effect == EFFECT_COIN || effect == EFFECT_EXTRA_LIFE;
}

/** `l_0525`: what the end of a tune does to the trigger registers. */
static void tune_ended(Work *work, int32_t effect)
{
    SoundState *sound = work->sound;
    switch (effect) {
    case EFFECT_COIN:
        /* One tune for every coin counted. */
        sound->triggers[effect]--;
        break;
    case EFFECT_NAME_ENTRY:
        sound->triggers[effect]--;
        if (sound->triggers[effect] == 0 || (sound->triggers[effect] & 1) != 0)
            sound->triggers[EFFECT_NAME_ENTRY_END] = 1;
        break;
    case EFFECT_PERFECT:
        /* At its end the "perfect" tune starts effect $13, the dive sound. */
        sound->triggers[effect] = 0;
        sound->triggers[EFFECT_DIVE] = 1;
        break;
    case EFFECT_CAPTURED_FIGHTER_HIT:
        for (int32_t i = 0; i < EFFECT_COUNT; i++) {
            if (!kept(i)) {
                sound->triggers[i] = 0;
                sound->playing[i] = 0;
            }
        }
        break;
    default:
        sound->triggers[effect] = 0;
        break;
    }
}

void tune(Work *work, int32_t effect)
{
    if (work->sound->triggers[effect] == 0)
        return;
    start_once(work, effect);
    if (play_tracks(work, effect)) {
        work->sound->playing[effect] = 0;
        tune_ended(work, effect);
    }
}

void set_volume(Work *work, int32_t voice, int32_t volume)
{
    work->mix.volumes[voice] = volume;
}
