# AI diary

The significant decisions made with Claude, most recent first.

| Date | Decision | Why |
| --- | --- | --- |
| 2026-10-25 | The whole repository in English: documentation, guides (with English file names), web pages (the installer defaults to English, Italian only for Italian browsers), pull requests | Luca: the project will be listed on an international site that collects FM-1 custom firmwares |
| 2026-10-25 | A preview of every PR on GitHub Pages in `pr-<number>/`, as in insert-coin: archive on the `gh-pages` branch, `preview.yml` on `ci`'s `workflow_run` (reusing the `site` artifact, no second build) and `pull_request_target` on close; it came with a separate PR to `main` | Luca's request: try the installer and the simulator before the merge. GitHub runs these workflows only from `main`'s copy, so they had to get there before the Space Invaders PR |
| 2026-10-25 | Browser installer (Luca's request on the PR): Felucca's protocol copied from Skiff into `installer/vendor/`, our own single page for every game, the `.fwsc` on the site with `games.json` (SHA-256, identity) | As with SLOOP and Skiff: install with Chrome and a cable, no Python; the protocol is the one Dinghy already has in the firmware, so nothing to invent |
| 2026-10-25 | MIDI ports paired by contained name, like SLOOP: the only change to Skiff's files | On Windows the output is called "MIDIOUT2 (Felucca)": Skiff's copy did not find the FM-1 |
| 2026-10-25 | No writing to a loader that is not ours (`ota-FM-1_9NN`), no going back to the official firmware from the page | SLOOP's caution: the official loader may lay the flash out differently; going back is M-UPGRADE's job |
| 2026-10-25 | Tests with a fake FM-1 in JavaScript (from the fake in Dinghy's `install_test.py`): the protocol in Node in CI, the page in Chromium locally | No synth in CI; the fake compares every answer byte by byte |
| 2026-10-24 | Summary guide 08 with the recipe for the next games; Galaga (224 × 288 on the screen) and the WSG sound remain open decisions | Whoever picks the project up, person or AI, knows where to start |
| 2026-10-24 | Simulator: the frame loop moved to `app.c`, shared by firmware and WebAssembly; a generic page that asks the game for its keys and buttons | The simulator plays exactly the firmware; Galaga and Breakout reuse the page |
| 2026-10-24 | WebAssembly with the system clang (`--target=wasm32 -nostdlib`), no Emscripten; a single page with the wasm in base64 | No extra tools in CI or locally; the artifact's page opens from disk |
| 2026-10-24 | Key and button presses accumulated between one frame and the next (on the firmware too) | A tap shorter than a frame was lost |
| 2026-10-24 | GitHub Pages published from `main` only; for PRs the `simulator` artifact (superseded on 2026-10-25 by the PR previews) | Pages has a single site; the single page is tried by downloading it |
| 2026-10-24 | A shared integer synthesizer (`libs/audio`) with the same data as `sound-bank.ts`; exponential curves in octaves with 2^x and log2 tables | The chip has no FPU; the web sounds are already a few numbers, it is enough to play them the same way |
| 2026-10-24 | Measured comparison: the real `synth.ts` in a Chromium `OfflineAudioContext` against the C code, envelope, volume and pitch (`make sounds`) | "Sounds the same" becomes a number; it led to the corrections of the square wave level (−1.5 dB) and of the filter (poles at 1.5 × the cutoff) |
| 2026-10-24 | A 16-slot queue between the main loop and the audio interrupt; high score saved only when the sounds are over | No interrupts to disable; erasing the flash also stops the audio |
| 2026-10-24 | Every frame redrawn from scratch into a second framebuffer, then `fb_present` copies only the changed rows | The same simple drawing as the web canvas; only the rows that differ go to the display |
| 2026-10-24 | PLAY for coin and start, FX for colour and black and white; attract legends with the synth's keys, 8 pixels higher | Buttons with LEDs close to the keyboard; row 240 does not reach the display |
| 2026-10-24 | High score in flash with `settings_save`, saved only after the game, `STORE_MAGIC` "INVD" | Erasing the flash stops the game for 50 ms; our own magic avoids reading other firmwares' data |
| 2026-10-24 | `config.h` tables moved to `config.c` | Including `config.h` in the drawing modules without unused-constant warnings |
| 2026-10-24 | Space Invaders' logic in C with the same modules and names as the TypeScript; structs by `const *`, returned by value; fixed arrays | Whoever knows insert-coin finds their way; the functional style stays without allocations. Full parity on the three traces at the first attempt |
| 2026-10-24 | Sprites as ASCII strings (`SPRITE_DEFINE`), converted to 32-bit-per-row bitmaps only for collisions (`libs/sprite`) | Drawings as readable as in the TypeScript; the conversion costs little and the library serves Galaga and Breakout too |
| 2026-10-24 | No debug mode in the port | It is for trying advanced rounds in the browser; on the synth it would complicate without helping |
| 2026-10-24 | Traces: one line per frame with controls and the FNV-1a hash of the state string; three games (idle, restless, aiming) | The web logic is deterministic: same controls, same game; the hash keeps the files small (1 MB) |
| 2026-10-24 | A Node recorder outside insert-coin, loading the TypeScript with insert-coin's Vite | The TypeScript stays in insert-coin; Vite is needed because the game reads `import.meta.env` |
| 2026-10-24 | Automatic player that dodges (bomb below row 120, within 18 pixels) | Of five attempts it is the only one that reaches the second round |
| 2026-10-24 | Screen: indexed framebuffer at the original resolution, row-by-row map to the display, only changed rows sent with two buffers | A whole image costs 31 ms, a frame has 16.7; the row map covers centring, cropping and skipping rows with a single mechanism |
| 2026-10-24 | Space Invaders at 1:1 pixels: rows 8-239 and 243-250 | The top 8 rows and the 3 below the green line are empty in the original |
| 2026-10-24 | Host mode on macOS: the `Makefile` renames the `.pool` section | Mach-O refuses section names without a segment, and `platform/` is not touched |
| 2026-10-24 | Step 1 straight to `main`; from step 2 branch and pull request | An empty repository does not accept pull requests: the first commit is the base |
| 2026-10-24 | Pure modules compiled one by one, only `main.c` includes `platform.c` | Testing the logic on the computer without the platform; the build checks that the modules touch no registers |
| 2026-10-24 | Package identity `FM-1_95N` for Space Invaders | Free in Dinghy's list; to be confirmed with the Firmware Hub before the release |
| 2026-10-24 | A new monorepo, C only, GPL-3.0; the TypeScript stays in insert-coin and only the reference traces come in here | To share with the community without mixing web games and firmware |
| 2026-10-24 | Space Invaders and Galaga keys: OP1-OP3 left, OP4-OP5 fire, OP6-PIT-GLO right; Breakout with the SELECT and KNOB1 encoders | Luca's choice: two hands on the keyboard, encoders as paddles |
| 2026-10-24 | The screen as a reusable layer, configured per game | After Space Invaders (224 × 256) come Breakout (228 × 208) and Galaga (224 × 288) |
| 2026-10-24 | A browser simulator as the last step, but mandatory | Trying without installing the firmware on the synth |
| 2026-10-24 | Dinghy as the base, a C port with the TypeScript as specification and parity checks | The chip has no FPU: JavaScript, automatic translation and ROM emulation are ruled out ([analysis](https://claude.ai/artifact/SsyDLc6sKQEehsGhHX6bCZ), in Italian) |
