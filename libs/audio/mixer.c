/* SPDX-License-Identifier: GPL-3.0-only */
#include "mixer.h"

Mixer mixer_silent(void)
{
    Mixer mixer;
    for (uint32_t i = 0; i < MIXER_VOICES; i++)
        mixer.voices[i] = voice_silent();
    mixer.next = 0;
    return mixer;
}

/** The index of a free voice, or the next one in turn when all are busy. */
static uint32_t pick_voice(const Mixer *mixer)
{
    for (uint32_t k = 0; k < MIXER_VOICES; k++) {
        const uint32_t i = (mixer->next + k) % MIXER_VOICES;
        if (voice_is_free(&mixer->voices[i]))
            return i;
    }
    return mixer->next;
}

void mixer_play(Mixer *mixer, const SoundEffect *effect)
{
    for (uint32_t layer = 0; layer < effect->count; layer++) {
        const uint32_t i = pick_voice(mixer);
        mixer->voices[i] = voice_start(&effect->layers[layer]);
        mixer->next = (i + 1u) % MIXER_VOICES;
    }
}

void mixer_stop(Mixer *mixer)
{
    *mixer = mixer_silent();
}

void mixer_render(Mixer *mixer, int32_t *mix, uint32_t n)
{
    for (uint32_t i = 0; i < MIXER_VOICES; i++)
        voice_render(&mixer->voices[i], mix, n);
}

int mixer_is_playing(const Mixer *mixer)
{
    for (uint32_t i = 0; i < MIXER_VOICES; i++)
        if (!voice_is_free(&mixer->voices[i]))
            return 1;
    return 0;
}
