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
  │                                    └── parity with TypeScript     fb_present → screen_push│
  │                                        (traces, parity_test)      mixer → audio_render   │
  └─────────────────────────────────────────────────────────────────────────────────────────┘
```

- **Pure logic** (`games/space-invaders/src/*.c`): every module receives the state as `const T *` and returns the new state. It is tested on the computer with `make test`, which also replays the three traces recorded from the web version.
- **Shared libraries** (`libs/`): screen, drawing, font, sprites and collisions, controls, 60 Hz clock, synthesizer, high score, traces. None of them knows Space Invaders.
- **Two shells**: `main.c` for the synth (Dinghy in `platform/`), `sim/sim.c` for the browser. Everything else is shared.

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

Galaga and Breakout will take the same road; every step reuses what is already there.

1. **The folder.** `games/<game>/` with `firmware.json` (a free `FM-1_9N` identity, name and colour of the update screen), `src/config.h` and `src/config.c` for the values that are choices (keys, colours, rows shown).
2. **The traces.** A recorder like `tools/traces/record-space-invaders.mjs` that loads the web version and writes the games to `games/<game>/traces/`; a `trace_state.c` that writes the C state in the same form.
3. **The logic.** One C module for every TypeScript logic file, with its tests; `parity_test.c` must turn green.
4. **Screen and controls.** `screen_map_centered()` or `screen_map_rows()` decides how the game's resolution fits in 240 × 240 (Breakout's 228 × 208 fits centred; Galaga's 224 × 288 must be cropped or compressed: that is the open decision). Breakout reads the SELECT and KNOB1 encoders with `fm1_enc_take()`.
5. **Sounds.** The numbers of its `sound-bank.ts` in `sounds.c`; Galaga uses Namco's WSG sound chip (`@arcade/wsg` in the web version), which will need porting to `libs/audio`.
6. **`app.c`, `main.c`, `sim/sim.c`.** Copied from Space Invaders and adapted: they are thin shells.
7. **CI.** Nothing to do: the jobs run over every `games/*/firmware.json` and `games/*/sim/sim.c`, and the game shows up in the web installer by itself.
