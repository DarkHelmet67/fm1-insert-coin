# 08 · Summary and next games

Space Invaders runs on the FM-1: the same logic as the web version, proven over 32,000 frames, 1:1 pixel graphics, the sounds of the same synthesizer measured against Web Audio, the high score in flash, and a browser simulator that runs the same code. This guide puts the pieces together and explains how to add the next game.

## How it fits together

```
              keys, buttons                                   SPI screen, speaker
                  │                                                        ▲
  ┌───────────────┴──────────────── main.c (FM-1) / sim.c (browser) ───────┴───────────────┐
  │                                                                                         │
  │   app_step ── input_controls ── game_step ── sounds_for          render_game            │
  │                                    │                                  │                  │
  │                     playing_step ──┼── fleet, bombs, ufo,          draw, font            │
  │                                    │   shields, collisions        (libs/screen, font)    │
  │                                    │                                  │                  │
  │                                    └── parity with TypeScript     fb_present → bands out │
  │                                        (traces, parity_test)      mixer → audio_render   │
  └─────────────────────────────────────────────────────────────────────────────────────────┘
```

- **Pure logic** (`games/space-invaders/src/*.c`): every module receives the state as `const T *` and returns the new state. It is tested on the computer with `make test`, which also replays the three traces recorded from the web version.
- **Shared libraries** (`libs/`): screen, drawing, font, sprites and collisions, controls, 60 Hz clock, synthesizer, high score, traces. None of them knows Space Invaders.
- **Two shells**: `launcher/src/main.c` for the synth (Dinghy in `platform/`), `launcher/sim/sim.c` for the browser, both running the launcher's console ([guide 10](10-launcher.md)). Everything else is shared.

## What is verified, and how

| What | Check |
| --- | --- |
| The logic is the web version's | `parity_test`: three games, 32,000 frames, every state with the same hash |
| Every module does what it says | 600+ checks in `make test`, translated from insert-coin's tests |
| The graphics | `render_test` and the host mode screenshots (`host-screens` artifact in CI) |
| The sounds | `make sounds`: envelope, volume and pitch against the real `synth.ts` in Chromium |
| The firmware builds | `firmware` job in CI, with the JieLi toolchain (`firmware` artifact) |
| The simulator works | `tools/sim/smoke.mjs` in CI, `tools/sim/test.mjs` with Playwright locally |
| The web installer speaks the protocol | `make installer-test` in CI with a fake FM-1, `tools/installer/page-test.mjs` with Playwright locally ([guide 09](09-installer.md)) |
| On the real synth | Still to try: install the `.fwsc` from the `firmware` artifact, or from the web installer of the pull request's preview |

On the synth the things no test on the computer can tell remain to be confirmed: smoothness at 60 Hz with the real display, LED brightness, speaker volume. The package identity (`FM-1_95`) must be agreed with the FM-1 Firmware Hub before publishing it there.

## Adding a game

Breakout and Galaga took this road ([guides 10-12](10-launcher.md)); since the launcher, a game is a cartridge of the one firmware.

1. **The folder.** `games/<game>/src/` with `config.h` and `config.c` for the values that are choices (keys, colours, rows shown), and the game's name in `launcher/firmware.json`.
2. **The traces.** A recorder like `tools/traces/record-galaga.mjs` that loads the web version and writes the games to `games/<game>/traces/`; a `trace_state.c` that writes the C state in the same form.
3. **The logic.** One C module for every TypeScript logic file, with its tests; `parity_test.c` must turn green.
4. **Screen and controls.** `screen_map_centered()` or `screen_map_rows()` decides how the game's resolution fits in 240 × 240 (Breakout centred, Galaga squeezed one row in six). The panel arrives as a `Panel`: keys, buttons and encoder clicks.
5. **Sounds.** Effects as data (`SoundEffect`, from its `sound-bank.ts`), or a Namco sound program driving `libs/audio/wsg` like Galaga.
6. **`app.c` and `cartridge.c`.** One frame of the cabinet, and the `Cartridge` struct the launcher reads; a `unity.c` that includes every module; the cartridge added to `launcher/src/cartridges.c`.
7. **CI.** Nothing to do: the jobs build the one firmware and the simulator, and the installer lists the games of `launcher/firmware.json`.
