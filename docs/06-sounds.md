# 06 · Sounds

The web version uses no audio files: every effect is described by a few numbers (a tone sliding from one frequency to another, or a puff of filtered noise, with a volume that holds and then falls) and is played by the browser's synthesizer, Web Audio. The numbers come from measurements on recordings of the original cabinet. On the synth there is no browser, and no floating point either: it needs a small integer synthesizer that, with the same numbers, sounds the same.

## The same numbers

`games/space-invaders/src/sounds.c` is `sound-bank.ts` line by line, in integer units: hertz, milliseconds, volume in thousandths.

```c
/* web: { kind: 'tone', wave: 'triangle', sweep: 'linear', from: 2660, to: 2420,
 *        duration: 0.33, hold: 0.16, fade: 'linear', volume: 0.15 } */
static const Sound ALIEN_HIT[] = {
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, CURVE_LINEAR, 2660, 2420, 0, 0, 330, 160, 150},
};
```

A field left at zero takes the web's default value (exponential curve, no filter, no delay, no hold), so the lines stay as short as in TypeScript.

*When* to play is translated from `sounds.ts` too: `sounds_for(&before, &after)` compares the state before and after a frame and returns the sounds as bits. One more shot fired is the laser, a newborn "alien" effect is the invader hit, a new pass of the formation is the next note of the march. The game logic knows nothing about audio, as on the web.

## An integer synthesizer: `libs/audio`

| File | What it does | Web Audio |
| --- | --- | --- |
| `sound.h` | Sounds as data: `Sound`, `SoundEffect` (several layers) | `sound.ts` |
| `voice.h/.c` | A sound being played: oscillator or noise, filter, envelope | `OscillatorNode`, `BiquadFilterNode`, `GainNode` |
| `mixer.h/.c` | 16 voices together | the destination that sums the nodes |
| `pow2.h/.c` | Powers and logarithms of two in fixed point | `exponentialRampToValueAtTime` |
| `audio_fm1.h` | The queue between the main loop and the audio interrupt | `AudioContext` |

### Oscillators and noise

The oscillator's phase is a 32-bit number that completes a full turn when it overflows: every sample adds a step proportional to the frequency (2³² / 44,100 ≈ 97,392 per hertz). The waveforms come out of the phase: the square wave is the top bit, the sawtooth the top bits as they are, the triangle the sawtooth folded in half. The noise is an xorshift generator: three shifts and three XORs per sample.

### Curves that are "even to the ear" without floating point

Web Audio slides frequencies and fades volumes exponentially: the ear hears intervals in octaves as equal, not in hertz. With integers the trick is to work in octaves: `log2_q16()` turns the start and the end into octaves (one octave = 65,536), the curve becomes a straight line, and `scale_pow2()` brings the result back to hertz or volume. Two 65-entry tables with interpolation between entries are enough.

Pitch and volume are recomputed every 32 samples (0.7 ms), not at every sample: the ear cannot hear the difference, the processor can. And products of a block count (a few thousand at most) and a Q15 or Q16 value stay within 32 bits, with no 64-bit division, which the chip lacks.

### The filter

The web's low-pass is a 12 dB per octave biquad. Here it is two one-pole filters in a row (y += (x − y) · a), which fall with the same slope; to lose 3 dB at the cutoff frequency, like the biquad, the poles sit at one and a half times the cutoff.

## The comparison with the web version

"Sounds the same" can be measured. `make sounds INSERT_COIN=../insert-coin`:

1. `tools/sounds/render_sounds.c` plays every effect with the C synthesizer and writes it to `build/sounds/c/`;
2. `tools/sounds/render-web-sounds.mjs` takes the numbers from `sound-bank.ts` and has them played by the web's **real** synthesizer (`packages/audio/src/synth.ts`) inside Chromium, with an `OfflineAudioContext` that computes the audio without speakers, and writes `build/sounds/web/`;
3. `tools/sounds/compare.py` compares the pairs in 10 ms windows: the shape of the envelope (correlation, 1 = identical), the overall volume and the pitch (from zero crossings, in 50 ms windows).

The result:

| Sound | Envelope (correlation) | Volume C / web | Pitch C / web |
| --- | --- | --- | --- |
| alienHit | 1.00 | +0.0 dB | +0 cent |
| cannonHit | 0.95 | -0.5 dB | noise |
| coin | 1.00 | +0.1 dB | +0 cent |
| extraLife | 1.00 | -0.0 dB | +0 cent |
| march0 | 0.88 | +1.0 dB | +0 cent |
| march1 | 0.86 | +1.2 dB | +0 cent |
| march2 | 0.94 | +1.1 dB | +0 cent |
| march3 | 0.94 | +0.9 dB | +0 cent |
| newRecord | 1.00 | +0.0 dB | +0 cent |
| shot | 1.00 | +0.0 dB | +0 cent |
| ufo | 1.00 | +0.0 dB | +14 cent |
| ufoHit | 1.00 | +0.1 dB | +0 cent |

Tones and sweeps match. The march notes have a slightly lower correlation only because at 52-69 Hz one period lasts longer than a 10 ms window and the RMS value swings with the phase; the pitch is exact. The noise differs by nature (the web uses `Math.random`) but has the same envelope and the same volume within half a decibel.

Two corrections came straight out of this measurement: the web's square wave is band-limited and normalised on its peaks, so its flat part is about 1.5 dB lower (`SQUARE_LEVEL`); and the filter poles at one and a half times the cutoff brought the explosion's noise back to the right level.

## On the synth

`main.c` connects the two halves (as in this step; step 7 moved the frame into `app.c`):

```c
const GameState previous = game;
game = game_step(&previous, &controls);
play_sounds(sounds_for(&previous, &game));     /* audio_play() for every bit */
...
static void app_audio(int32_t *out, uint32_t n)
{
    audio_render(out, n);                      /* in the interrupt, every 5.8 ms */
}
```

Between the main loop and the interrupt there is a 16-slot queue: only the loop writes the write index, only the interrupt the read index, so neither has to stop the other. The output is mono, the same on both channels, like the cabinet's speaker; the synth's MASTER knob sets the volume.

The high score is saved only when no sound is playing (`audio_is_playing()`): erasing the flash also stops the audio interrupt for about 50 ms, and you would hear it.

In host mode the sound ends up in `build/host/space-invaders.wav`: the test script lets you hear the coin, the march, the shots and the invaders being hit.
