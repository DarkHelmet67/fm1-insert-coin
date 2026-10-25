# License and credits

fm1-insert-coin is free software under the GNU General Public License, version 3 only (`GPL-3.0-only`, full text in [LICENSE](LICENSE)). You may use, study, change and share it; anyone who distributes a firmware built from here must also give the complete source code, under the same license.

## Who made what

| Part | Authors |
| --- | --- |
| `platform/`: the firmware platform (boot, USB, updates, MIDI, audio, screen, flash, host mode) | [Dinghy](https://github.com/zvenson/dinghy) by zvenson, on [Felucca](https://github.com/hugelton/Felucca) by Leo Kuroshita (Hügelton Instruments) with the changes of [SLOOP](https://github.com/isod89/sloop-fm1) (isod89) and the SMK-37 layer by M. Geisler (genkigenki); details in [platform/LICENSING.md](platform/LICENSING.md) |
| `tools/build.py` | Adapted from `platform/tools/build.py` (Leo Kuroshita, zvenson) |
| `installer/vendor/`: the USB-MIDI update protocol and the `.fwsc` packages in the browser | Leo Kuroshita (Hügelton Instruments), copied from [Skiff](https://github.com/illuminus5/skiff); details in [installer/UPSTREAM.md](installer/UPSTREAM.md) |
| `.github/actions/publish-pages` | From [insert-coin](https://github.com/DarkHelmet67/insert-coin) (same author) |
| `libs/`, `games/`, `docs/`, everything else | Luca (DarkHelmet67), with Claude |

The games are ports of the web version in [insert-coin](https://github.com/DarkHelmet67/insert-coin) (MIT, same author): drawings, sounds and rules are rebuilt from research on the original hardware and software, with no ROM.

## Third-party material

| What | License | Where |
| --- | --- | --- |
| Terminus 8x16 font in `platform/firmware/gen/felucca_font.h` | SIL OFL 1.1 | `platform/assets/fonts/Terminus-LICENSE.txt` |
| JieLi AC79 SDK: three files read at build time and put in the package, none of them in this repository | Apache-2.0 | <https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK> |

## Trademarks

"Space Invaders" is a trademark of Taito; "Galaga" and "Pac-Man" of Bandai Namco; "Breakout" of Atari. "M-VAVE" and "FM-1" belong to their respective owners. This hobby and teaching project is not affiliated with or endorsed by any of them.
