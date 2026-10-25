/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Renders every sound effect of a game with the C synthesizer (libs/audio) to WAV files, the
 * same length as render-web-sounds.mjs, for compare.py. Built and run by `make sounds`.
 *
 *   render_sounds OUT_DIR
 */
#include <stdio.h>

#include "audio/mixer.h"
#include "sounds.h"

/** The names of the web version's sound bank, in the order of SoundName. */
static const char *const NAMES[SOUND_COUNT] = {
    "coin", "shot", "alienHit", "ufo", "ufoHit", "cannonHit", "extraLife", "newRecord",
    "march0", "march1", "march2", "march3",
};

/** 1.2 seconds, as the web renders. */
#define LENGTH (AUDIO_RATE * 6 / 5)

static int32_t mix[LENGTH];
static Mixer mixer;

/** Writes a little-endian number of `bytes` bytes. */
static void put(FILE *f, uint32_t value, int bytes)
{
    for (int i = 0; i < bytes; i++)
        fputc((int)(value >> (8 * i) & 0xFFu), f);
}

/** Writes the mix as a 16-bit mono WAV file. */
static int write_wav(const char *path)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return 0;
    fputs("RIFF", f);
    put(f, 36u + LENGTH * 2u, 4);
    fputs("WAVEfmt ", f);
    put(f, 16, 4);
    put(f, 1, 2);
    put(f, 1, 2);
    put(f, AUDIO_RATE, 4);
    put(f, AUDIO_RATE * 2u, 4);
    put(f, 2, 2);
    put(f, 16, 2);
    fputs("data", f);
    put(f, LENGTH * 2u, 4);
    for (uint32_t i = 0; i < LENGTH; i++) {
        const int32_t s = mix[i] > 32767 ? 32767 : mix[i] < -32767 ? -32767 : mix[i];
        put(f, (uint32_t)s, 2);
    }
    return fclose(f) == 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: render_sounds OUT_DIR\n");
        return 1;
    }
    for (uint32_t name = 0; name < SOUND_COUNT; name++) {
        char path[256];
        for (uint32_t i = 0; i < LENGTH; i++)
            mix[i] = 0;
        mixer = mixer_silent();
        mixer_play(&mixer, &SOUND_EFFECTS[name]);
        mixer_render(&mixer, mix, LENGTH);
        snprintf(path, sizeof path, "%s/%s.wav", argv[1], NAMES[name]);
        if (!write_wav(path)) {
            fprintf(stderr, "cannot write %s\n", path);
            return 1;
        }
    }
    return 0;
}
