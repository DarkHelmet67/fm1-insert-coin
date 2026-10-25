# 00 Introduction

This repository brings the games of [insert-coin](https://github.com/DarkHelmet67/insert-coin), web remakes of arcade classics, to real hardware: the M-Vave FM-1 pocket synthesizer. It is the project's sequel: the same faithfulness to the original, the same method, but on a chip with no browser, no JavaScript and no floating point.

## Why a synthesizer

In 2026 M-Vave opened the FM-1's firmware ([fm1_open](https://github.com/m-vave/fm1_open)) and within a few weeks the community wrote more than a hundred alternative firmwares. Some are games: [FM-1 Doom](https://github.com/zvenson/fm1doom) runs a Freedoom level, [picovave](https://github.com/acidsound/picovave) runs PICO-8 cartridges. The FM-1 has everything a pocket arcade needs:

| | |
| --- | --- |
| Processor | JieLi AC791N, two cores, no floating-point unit |
| Memory | 1 MiB of flash, about 430 KB of RAM |
| Screen | 240 × 240 colour, on an SPI bus |
| Controls | 27 keys with LEDs, 14 buttons, 7 encoders |
| Sound | Speaker, headphone output, 44.1 kHz |

## Why C, and not TypeScript

The chip has no FPU, and in JavaScript every number is floating point: a JS interpreter would have to emulate the arithmetic in software, with little RAM and nobody having ever tried it on this hardware. Translating the TypeScript to C automatically does not work with our style (immutable objects, higher-order functions). Emulating the original board with its ROM cannot be distributed and goes against our approach.

That leaves the most honest road: rewrite the logic in C, keeping the TypeScript as an **executable specification**. The web game records its state at every frame for a fixed sequence of keys; the C code, in host mode, receives the same keys and must produce the same state. If the two traces match, it is the same game, and a test proves it.

## Why Dinghy

[Dinghy](https://github.com/zvenson/dinghy) is the FM-1's minimal firmware: a sine wave on the keys and, underneath, everything you need (boot, USB updates, rescue, keys, screen, audio, flash memory). An app writes two functions, `app_main()` and `app_audio()`, and host mode runs them on the computer. Its author used it for Doom, so it is a base proven on the device. The alternative, fm1_open, is a complete synthesizer with a closed binary engine and a Windows-only toolchain.

## The plan for the first game

Completed on 24 October 2026; every step has its guide.

1. The monorepo, Dinghy, the host build and the firmware build in CI; the first firmware with the title and the keys lit ([guide 01](01-platform.md)).
2. The shared libraries: screen and 60 Hz clock ([guide 02](02-screen.md)).
3. The reference traces recorded from the TypeScript version ([guide 03](03-traces.md)).
4. Space Invaders' logic in C, module by module, with parity green ([guide 04](04-logic.md)).
5. Drawing, controls, attract screen, game, high score: the first playable `.fwsc` ([guide 05](05-graphics-and-controls.md)).
6. The sounds ([guide 06](06-sounds.md)).
7. The browser simulator ([guide 07](07-simulator.md)).
8. The final documentation ([guide 08](08-summary.md)).

After the plan, asked for by Luca while reviewing the pull request: the browser installer, to install without the command line, and a preview of every pull request on GitHub Pages ([guide 09](09-installer.md)).

The analysis and the decisions made before starting are in the [AI diary](ai-workflow.md).
