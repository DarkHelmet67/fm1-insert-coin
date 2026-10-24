# SPDX-License-Identifier: GPL-3.0-only
"""Compares the sound effects of the C synthesizer with the web version's (docs/06-suoni.md).

    python3 tools/sounds/compare.py WEB_DIR C_DIR

For every WAV file present in both folders it measures, in windows of 10 ms:
- the loudness (RMS): how alike the two volume envelopes are (correlation, 1 = the same shape)
  and how much louder or softer the C version is overall, in dB;
- the pitch, from the zero crossings in windows of 50 ms where both are loud enough: the median
  ratio C / web, in cents (100 cents = a semitone).
Noise has no pitch: its line shows only the loudness.
"""
import math
import struct
import sys
import wave
from pathlib import Path

WINDOW = 441  # 10 ms at 44.1 kHz
PITCH_WINDOWS = 5  # the pitch over 50 ms: the march notes (52-69 Hz) cross zero rarely


def samples(path):
    with wave.open(str(path)) as w:
        frames = w.readframes(w.getnframes())
        data = struct.unpack("<%dh" % (len(frames) // 2), frames)
        return data[:: w.getnchannels()]


def windows(data):
    return [data[i : i + WINDOW] for i in range(0, len(data) - WINDOW + 1, WINDOW)]


def rms(window):
    return math.sqrt(sum(x * x for x in window) / len(window))


def crossings(window):
    return sum((a >= 0) != (b >= 0) for a, b in zip(window, window[1:]))


def correlation(a, b):
    ma, mb = sum(a) / len(a), sum(b) / len(b)
    num = sum((x - ma) * (y - mb) for x, y in zip(a, b))
    den = math.sqrt(sum((x - ma) ** 2 for x in a) * sum((y - mb) ** 2 for y in b))
    return num / den if den else 1.0


def compare(web, c, noise):
    ww, cw = windows(web), windows(c)
    we, ce = [rms(w) for w in ww], [rms(w) for w in cw]
    level = 20 * math.log10((sum(ce) + 1e-9) / (sum(we) + 1e-9))
    loud = max(we) * 0.1
    step = PITCH_WINDOWS * WINDOW
    ratios = sorted(
        crossings(c[i : i + step]) / crossings(web[i : i + step])
        for i in range(0, len(web) - step + 1, step)
        if rms(web[i : i + step]) > loud and rms(c[i : i + step]) > loud
        and crossings(web[i : i + step]) > 4
    )
    cents = None if noise or not ratios else 1200 * math.log2(ratios[len(ratios) // 2])
    return correlation(we, ce), level, cents


def main(web_dir, c_dir):
    print("| Suono | Inviluppo (correlazione) | Volume C / web | Altezza C / web |")
    print("| --- | --- | --- | --- |")
    for web_file in sorted(Path(web_dir).glob("*.wav")):
        c_file = Path(c_dir) / web_file.name
        if not c_file.exists():
            continue
        name = web_file.stem
        corr, level, cents = compare(samples(web_file), samples(c_file), name == "cannonHit")
        pitch = "rumore" if cents is None else "%+.0f cent" % cents
        print("| %s | %.2f | %+.1f dB | %s |" % (name, corr, level, pitch))


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    main(sys.argv[1], sys.argv[2])
