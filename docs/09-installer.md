# 09 · The browser installer

To install a game on the synth all you need is Chrome (or Edge) and a USB cable: open the [installer page](https://darkhelmet67.github.io/fm1-insert-coin/installer/), choose the game and press **Install**. No Python, no programs to download. It is the same convenience as the installers of [SLOOP](https://isod89.github.io/sloop-fm1/webapp/installer/) and [Skiff](https://illuminus5.github.io/skiff/webapp/installer/), asked for by Luca while reviewing the Space Invaders pull request.

<p align="center"><img src="images/installer.png" width="480" alt="The installer page at the end of an install, with Space Invaders chosen"></p>

## How it talks to the synth

The browser sees the FM-1 as a MIDI device (WebMIDI, with SysEx permission). The update is Felucca's protocol, which Dinghy already carries in `platform/` and which `platform/tools/fm1_install.py` also speaks from the command line. It works in two stages, and in both it is the synth that asks for pieces of the package, 512 bytes at a time, and the page that answers:

| Stage | Who asks | What happens |
| --- | --- | --- |
| 0 | The page | Identity query: the FM-1 answers, for example `FM-1_015` (the official V15 firmware) |
| 1 | The firmware running now | Reads a few pieces of the package and restarts as the update loader (for Space Invaders `ota-FM-1_950`, USB port "Felucca Update") |
| 2 | The loader | Reads the whole package and writes it to flash; the game's header last |
| 3 | The game | The FM-1 restarts and the page checks that it reports the package's identity (`FM-1_950`) |

Stage 1 works from the official V15 firmware just as from one of our games: that is why the install starts from any state. If the cable comes out during stage 2, the FM-1 stays in the loader (with the header written last, a half-written game never starts): the page waits for it to come back and carries on; if the page was closed, the next press of Install finds the FM-1 in the loader and finishes the write.

## Our code and other people's

| File | What it does | By |
| --- | --- | --- |
| `installer/vendor/fm1ota.js` | The protocol: identity, the two stages, resuming after a drop, the percentage | Leo Kuroshita (Felucca), from Skiff, GPL-3.0 |
| `installer/vendor/fm1pkg.js` | The `.fwsc` package: identity, logical image, files and CRCs | Leo Kuroshita (Felucca), from Skiff, GPL-3.0 |
| `installer/package.js` | Downloads the game's `.fwsc` and checks it (size, SHA-256, identity) before the synth sees a byte of it | Ours |
| `installer/installer.js`, `texts.js`, `index.html`, `style.css` | The page: list of games, button, progress bar, messages in English (and Italian for Italian browsers) | Ours |

The two Skiff modules are copied as they were, with a single change noted in [installer/UPSTREAM.md](../installer/UPSTREAM.md): on Windows the FM-1's MIDI output is called "MIDIOUT2 (Felucca)", not "Felucca", and Skiff's copy did not recognise it. Now an output counts if its name is the input's or contains it, as SLOOP's installer does.

Two cautious rules, taken from SLOOP: the page never writes to a device that is not an FM-1 (an SMK-37, for example), and it does not finish an update started by the official firmware (its loader, `ota-FM-1_015`, may lay the flash out differently): that one is finished with M-VAVE's M-UPGRADE.

## One page for every game

The page does not know Space Invaders. It reads `games.json`, written by `tools/installer/bundle.py` with one line per package built:

```json
{ "game": "space-invaders", "name": "SPACE INVADERS", "product": "FM-1_950",
  "pkg": "../firmware/space-invaders.fwsc", "sha256": "…", "size": 123456 }
```

`bundle.py` checks every package as `fm1_install.py` checks it (identity, Felucca's loader inside, header, file list and the CRC of each file) and copies it to `firmware/`. When Galaga and Breakout arrive they will show up in the list by themselves, with the link to the simulator and to the `.fwsc` to download.

## On the site

The GitHub Pages site is now built by three CI jobs:

```
firmware ──┐                      ┌── publish ── pages          (push to main: the root of the site)
           ├── site (artifact) ───┤
simulator ─┘                      └── preview.yml               (PR ready: the pr-<number>/ folder)
```

The `site` job downloads the `simulator` and `firmware` artifacts, adds the installer and the packages with `bundle.py` and uploads the result as the `site` artifact. Layout:

```
index.html                  the list of games and the link to the installer
space-invaders/index.html   the simulator
installer/                  the page, its modules, games.json
firmware/space-invaders.fwsc
```

### The preview of a pull request

To try on the synth before the merge (Luca's request), every pull request ready for review gets its own site at `https://darkhelmet67.github.io/fm1-insert-coin/pr-<number>/`, with the pull request's simulator, installer and packages; a comment on the pull request gives the link. It is the same design as insert-coin's previews:

- **An archive.** Pages publishes one whole folder at a time: to keep `main`'s site and the previews together, the published site lives on the `gh-pages` branch. `.github/actions/publish-pages` changes only its own part: the root for `main` (the `publish` job of `ci.yml`), `pr-<number>/` for a pull request, then the whole archive is published.
- **Who publishes.** `.github/workflows/preview.yml` starts when `ci` ends green on a pull request of this repository, open and not a draft, still at the commit that was built: it downloads that run's `site` artifact and copies it to `pr-<number>/`. When the pull request is closed, it removes the folder and updates the comment.
- **Safety.** `preview.yml` always runs from `main`'s copy (`workflow_run` and `pull_request_target`), with write permissions, and never runs the pull request's code: it only copies the built files. Pull requests from forks get no preview.
- **One step at a time.** Every publication shares the `pages` concurrency group.

The installer works from the preview too: WebMIDI wants HTTPS, and Pages is HTTPS.

The page loads JavaScript modules and the `.fwsc` with `fetch`, so it needs a server: from GitHub Pages it works, opened from disk it does not. Locally:

```sh
make sim GAME=space-invaders
python3 tools/build.py space-invaders     # or the .fwsc from the firmware artifact
make installer                            # every build/*/*.fwsc -> build/site/installer/
python3 -m http.server -d build/site      # then http://localhost:8000/installer/
```

WebMIDI also works on `localhost`, so the local page can install on a real FM-1.

## Trying it without a synth

The protocol is tested with a fake FM-1, `tools/installer/fake_fm1.mjs`, which replaces WebMIDI and behaves like the one in `platform/tests/install_test.py`: it answers the identity query, asks for the pieces in the two stages, restarts under different USB names, can pull the cable halfway.

- **`make installer-test`** (also in CI, in the `host` job): the package checks and five complete installs. From the official V15 firmware; with Windows' port names; with the cable pulled halfway through the write, then resumed; from an update cut off earlier; on an SMK-37, refused before anything is sent. Every answer to the synth is compared byte by byte with the package.
- **`tools/installer/page-test.mjs`** (Playwright, locally): the real page in Chromium with the fake FM-1 instead of WebMIDI, all the way to "Done!". The fake package is written by `tools/installer/fake_package.py`:

```sh
python3 tools/installer/fake_package.py FM-1_950 build/fake/space-invaders.fwsc
make sim GAME=space-invaders && make installer FWSC=build/fake/space-invaders.fwsc
node tools/installer/page-test.mjs build/site
```

The fake does not replace a test on the real synth: it shows that the page speaks the protocol like `fm1_install.py`, which Dinghy has already tested on the FM-1.
