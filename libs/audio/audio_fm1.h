/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The sound on the FM-1: the main loop asks for effects with audio_play() and gives the voices
 * of a WSG chip with audio_voices(); the sound interrupt mixes them with audio_render(), called
 * from app_audio(). Include from main.c, after platform.c.
 *
 * The two sides meet in small queues: only the main loop writes `audio_written` and
 * `audio_frames_written`, only the interrupt the two read counts, so neither needs to switch the
 * other off.
 *
 * The chip's voices are queued frame by frame and the interrupt plays each for exactly a 60th of
 * a second of samples, whenever the main loop got round to them. On the synth the main loop is
 * often late: drawing a whole Galaga screen holds it up, and then it catches up with several
 * frames at once. Played as they came, those frames would be cut or lost and the music would
 * stumble; queued, they sound as the game made them, a frame or two later.
 */
#ifndef AUDIO_FM1_H
#define AUDIO_FM1_H

#include "audio/mixer.h"
#include "audio/wsg.h"

/** Effects asked for and not yet started: more than a frame ever asks for. */
#define AUDIO_QUEUE 16u

/** The frames the platform asks for at once (platform.c: HALF_FRAMES). */
#define AUDIO_BLOCK 256u

static const SoundEffect *volatile audio_queue[AUDIO_QUEUE];
static volatile uint32_t audio_written, audio_read;
static Mixer audio_mixer;
static int32_t audio_mix[AUDIO_BLOCK];

/** Frames of voices the queue holds: more than the main loop ever catches up at once. */
#define AUDIO_FRAMES 8u

/** After the queue ran dry, the frames it waits for before playing again (to absorb the jitter). */
#define AUDIO_FRAMES_REFILL 2u

/** The voices queued by the main loop, and the waveform PROM of the game that plays them. */
static WsgFrame audio_frames[AUDIO_FRAMES];
static volatile uint32_t audio_frames_written, audio_frames_read;
static const uint8_t *volatile audio_prom;
/** The interrupt's own: the frame it plays, the chip playing it, whether it waits for a refill. */
static WsgFrame audio_frame;
static Wsg audio_wsg;
static int audio_starved = 1;

/** Stops the compiler moving memory writes across this point (the slot before the count). */
#define AUDIO_BARRIER() __asm__ volatile("" ::: "memory")

/**
 * In the main loop, once a frame: what the WSG chip plays in that frame, and its waveform PROM;
 * `frame` NULL (a game without the chip, the menu) is silence. Dropped if the queue is full.
 */
static void audio_voices(const WsgFrame *frame, const uint8_t *prom)
{
    const uint32_t w = audio_frames_written;
    audio_prom = prom;
    if (w - audio_frames_read >= AUDIO_FRAMES)
        return;
    audio_frames[w % AUDIO_FRAMES] = frame != 0 ? *frame : wsg_silent_frame();
    AUDIO_BARRIER();
    audio_frames_written = w + 1u;
}

/**
 * In the interrupt, when a frame has played whole: starts the next one queued. When the queue is
 * empty the last frame's second tick holds, and playing starts again only once a few frames are
 * queued, so that a main loop running a little late does not leave a gap at every frame.
 */
static void audio_next_frame(void)
{
    const uint32_t queued = audio_frames_written - audio_frames_read;
    if (queued == 0u)
        audio_starved = 1;
    if (queued == 0u || (audio_starved && queued < AUDIO_FRAMES_REFILL))
        return;
    audio_starved = 0;
    audio_frame = audio_frames[audio_frames_read % AUDIO_FRAMES];
    audio_frames_read++;
    wsg_start_frame(&audio_wsg);
}

/** In the interrupt: adds `n` samples of the chip to `mix`, frame after frame from the queue. */
static void audio_render_voices(const uint8_t *prom, int32_t *mix, uint32_t n)
{
    for (uint32_t done = 0; done < n;) {
        if (wsg_frame_left(&audio_wsg) == 0u)
            audio_next_frame();
        const uint32_t left = wsg_frame_left(&audio_wsg);
        const uint32_t count = left == 0u || left > n - done ? n - done : left;
        wsg_render(&audio_wsg, &audio_frame, prom, mix + done, count);
        done += count;
    }
}

/** Asks for `effect` to start at the next sound interrupt (dropped if the queue is full). */
static void audio_play(const SoundEffect *effect)
{
    const uint32_t w = audio_written;
    if (w - audio_read >= AUDIO_QUEUE)
        return;
    audio_queue[w % AUDIO_QUEUE] = effect;
    audio_written = w + 1u; /* after the slot is filled: the interrupt may read it from now on */
}

/** Whether a sound is playing (read from the main loop: a moment's delay does not matter). */
static int audio_is_playing(void)
{
    return audio_written != audio_read || mixer_is_playing(&audio_mixer);
}

/** In app_audio(): starts the effects asked for, then fills `n` stereo frames (Q15). */
static void audio_render(int32_t *out, uint32_t n)
{
    while (audio_read != audio_written) {
        mixer_play(&audio_mixer, audio_queue[audio_read % AUDIO_QUEUE]);
        audio_read++;
    }
    const uint8_t *prom = audio_prom;
    for (uint32_t done = 0; done < n;) {
        const uint32_t count = n - done < AUDIO_BLOCK ? n - done : AUDIO_BLOCK;
        for (uint32_t i = 0; i < count; i++)
            audio_mix[i] = 0;
        mixer_render(&audio_mixer, audio_mix, count);
        if (prom != 0)
            audio_render_voices(prom, audio_mix, count);
        else
            audio_frames_read = audio_frames_written; /* no chip in this game: nothing to play */
        for (uint32_t i = 0; i < count; i++) {
            out[2u * (done + i)] += audio_mix[i]; /* the same on both sides: mono, as the cabinet */
            out[2u * (done + i) + 1u] += audio_mix[i];
        }
        done += count;
    }
}

#endif
