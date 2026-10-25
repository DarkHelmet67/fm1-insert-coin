# 01 The platform

The first step sets up what you need to write a game without touching the hardware: the Dinghy platform, the firmware build, host mode, the tests and CI. The result is a firmware with the Space Invaders title and the eight game keys lit.

<p align="center"><img src="images/space-invaders-step1.png" width="240" alt="The title with fire pressed"></p>

## How an app runs on Dinghy

A Dinghy firmware is a single compilation unit: the app's file includes `platform.c` and defines two functions.

```c
#include "platform.c"

static void app_audio(int32_t *out, uint32_t n);   /* the audio interrupt: n stereo samples to fill */
static void app_main(void);                        /* the main loop: never returns */
```

`app_main()` must call `platform_poll()` often: it keeps the watchdog alive (8 seconds, then the FM-1 restarts) and serves USB, and with it updates. The keys are bits of `fm1_in.notes` (bit 0 = F3, bit 26 = G5), the LEDs light with `fm1_led_key()`, text is written with `draw_text_box()`. The full reference is [platform/docs/API.md](../platform/docs/API.md); the chip's rules (no `float`, no 64-bit division, what not to do in the audio interrupt) are in [platform/docs/RULES.md](../platform/docs/RULES.md).

## The boundary with the hardware

In this repository only `games/<game>/src/main.c` includes `platform.c`. Everything else, in `games/<game>/src/` and in `libs/`, is pure C: no registers, no screen, no global variables. `tools/build.py` compiles it file by file and links it into the firmware; `make test` compiles it on the computer with its tests.

The first pure module is the key map, in `libs/input/keymap.c`: given the set of keys held, it returns the actions (left, right, fire). Left and right together cancel out, as in the web version. Each game's keys live in its `config.h` (in step 1 as a `static const`, later moved to `config.c`):

```c
static const KeyMap CONTROLS = {
    .left = FM1_KEY_BIT(FM1_KEY_OP1) | FM1_KEY_BIT(FM1_KEY_OP2) | FM1_KEY_BIT(FM1_KEY_OP3),
    .fire = FM1_KEY_BIT(FM1_KEY_OP4) | FM1_KEY_BIT(FM1_KEY_OP5),
    .right = FM1_KEY_BIT(FM1_KEY_OP6) | FM1_KEY_BIT(FM1_KEY_PIT) | FM1_KEY_BIT(FM1_KEY_GLO),
};
```

The names OP1...GLO are the labels printed on the black keys. Which bit each one is comes from the `edit_keys` table of the official firmware ([app/screen.c in fm1_open](https://github.com/m-vave/fm1_open/blob/main/app/screen.c)): OP1 is F#3, bit 1.

## The tests

A test is a program: `keymap_test.c` next to `keymap.c`, with a `main()` that runs the cases one by one. The harness is `libs/test/check.h`, a few lines: `CHECK(condition)` and `CHECK_EQ(value, expected)` report a failure without stopping the test, so one run shows every error.

```sh
make test
```

## Host mode

`make host GAME=space-invaders` compiles the game with `-DDINGHY_HOST`: `platform.c` then includes `platform/tests/host.c`, a fake FM-1. Every call to `platform_poll()` is one millisecond; a script presses keys over time and asks for screenshots.

```
# games/space-invaders/scripts/title.txt
500 shot
600 keys 0x2          # OP1 (bit 1): left
900 shot
1400 keys 0x22102     # OP1 + OP4 + GLO: left and right cancel out, fire remains
1700 shot
2000 end
```

Out come `build/host/space-invaders-<ms>.png` and `build/host/space-invaders.wav`.

## The firmware build

`python3 tools/build.py space-invaders` does what Dinghy's `build.sh` did, for a game folder: it compiles `main.c` with the platform and the pure modules, links, builds the update loader and packs the `.fwsc`. Before writing the package it checks for what would brick the FM-1: an image too large, too much RAM, a call into the chip's ROM, a register touched outside `platform/firmware/hal/`.

Every game has its `firmware.json`:

```json
{ "identity": "FM-1_95", "screen_name": "SPACE INVADERS", "screen_colour": "0x07E0" }
```

The identity is the name by which the FM-1, the installers and the Firmware Hub recognise the firmware; the last digit is the minor version (`FM-1_950` for development builds). The `9NN` identities already taken are listed in [platform/docs/MAKE-IT-YOURS.md](../platform/docs/MAKE-IT-YOURS.md); `95N` must be confirmed with the Firmware Hub before the first release.

## CI

`.github/workflows/ci.yml` runs on every push and pull request. The first job checks the formatting, runs the tests and runs every game in host mode, attaching the screenshots. The second downloads the JieLi toolchain and the AC79 SDK and builds every game's `.fwsc`, attaching it to the run: that is where you download the firmware to install. (Later steps added the simulator, the site and the previews: see guides 07 and 09.)
