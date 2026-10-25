# Development guidelines (humans and AI)

These rules apply to everyone who writes code in this repository, people or AI assistants. Some are checked automatically (`make format-check`, warnings treated as errors, the checks in `tools/build.py`); the others are checked in review.

## AI for humans

The code of this project will be read by people: **code for humans, not for AI**.

- Use well-known patterns and simple C, not "clever" solutions.
- No monolithic code: small files and functions, each with a single responsibility. A file past a couple of hundred lines should be split.
- Many small helper functions, easy to unit test.
- Comment the more complex parts, explaining the _why_, not the _what_.

## Development rules

| Rule | How it applies | Check |
| --- | --- | --- |
| Pure logic, separate from the hardware | Only each game's `main.c` talks to the FM-1 (it includes `platform.c`), helped by the `libs/*/*_fm1.h` files it includes after `platform.c`. Every `.c` file in `games/*/src/` and `libs/` is C with no hardware, compiled on its own and tested on the computer | `tools/build.py` compiles these modules one by one; registers are touched only in `platform/firmware/hal/` |
| Functional style | State is passed as `const T *` and a function returns the new state (`GameState game_step(const GameState *s, uint32_t actions)`); no global variables in pure modules | Review |
| Integers only | No `float` or `double`, no 64-bit division: the chip has no FPU and no libgcc. Fixed point (Q15, Q16) where needed | The firmware link fails |
| Every function, struct and enum has a Doxygen comment | `/** ... */` with a line saying what it does; `@param`/`@returns` when they are not obvious | Review |
| Zero warnings | `-std=c99 -Wall -Wextra -Werror` for the tests on the computer | `make test` |
| Formatting | clang-format with `.clang-format` | `make format-check` |
| Uncertain values in a single file | Everything that is a choice rather than a fact of the original (keys, colours, estimated timings) lives in `games/<game>/src/config.h` | Review |

### Allowed exceptions

- **`main.c` is the imperative shell.** The main loop, the screen, the LEDs and the audio interrupt keep state in `static` variables: it is the boundary with the hardware, like the web version's game loop.
- **The `*_fm1.h` files.** Glue code reused by several games (for example sending the screen to the display) lives in `libs/<module>/<name>_fm1.h`, with `static` functions: it is included only from `main.c`, after `platform.c`, and never compiled on its own.
- **The framebuffer.** Drawing means writing pixels into a buffer: the drawing functions receive the buffer to change, and only that.
- **The synthesizer voices.** `voice_render()` and `mixer_render()` run in the audio interrupt, 44,100 times a second: they receive the voice (or the mixer) and the buffer to change, like drawing. Choosing *which* sounds to play stays pure (`sounds_for()`).
- **`platform/` belongs to Dinghy.** It is copied as it comes from the original project and follows neither our rules nor our style; every change must be noted in `platform/UPSTREAM.md`.
- **Tests.** Mutable fixtures are allowed in `*_test.c` files.
- **The simulator.** `games/<game>/sim/sim.c` is the browser shell, as `main.c` is the synth's: state in `static` variables, functions exported with `SIM_EXPORT`. `sim/sim.js` is JavaScript for the page.
- **The web installer.** `installer/*.js` is JavaScript for the page; `installer/vendor/` belongs to Felucca (from Skiff) and, like `platform/`, is copied as it comes: every change must be noted in `installer/UPSTREAM.md`.
- **The trace recorder.** `tools/traces/*.mjs` is JavaScript for Node: it loads the TypeScript version from a checkout of insert-coin and writes the traces. It is not part of the firmware.

If a rule becomes a real obstacle, discuss it and update this file.

## Structure and conventions

- **Everything in English:** code, comments, documentation (`README.md`, `docs/`), web pages, commit messages and pull requests. The project is shared with the international FM-1 community.
- `platform/` is Dinghy, `libs/` our code shared by the games, `games/<game>/` one game with its `firmware.json` (package identity, name on the update screen).
- Shared code in `libs/` never includes from `games/`.
- Every game is a faithful port of the web version in [insert-coin](https://github.com/DarkHelmet67/insert-coin): the TypeScript logic is the specification, and parity is checked with traces recorded frame by frame. The TypeScript does not enter this repository: only the traces do, as test files.
- Every new module comes with its tests (`module_test.c` next to `module.c`) and with its guide in `docs/`.
- Every significant decision made with the AI goes in the [AI diary](docs/ai-workflow.md).
- Every significant change goes in the [single prompt](docs/single-prompt.md), which must always be able to recreate the project as it is.

## License

The whole repository is GPL-3.0, like Dinghy, which it derives from: whoever receives a `.fwsc` is entitled to the source. Every new C file starts with `/* SPDX-License-Identifier: GPL-3.0-only */`. The credits are in [LICENSING.md](LICENSING.md).

## Before every commit

```bash
make format-check && make test && make host GAME=<game>
```

The firmware (`python3 tools/build.py <game>`) needs the JieLi toolchain; GitHub Actions builds it anyway on every push.
