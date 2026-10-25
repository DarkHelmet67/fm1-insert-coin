# Where platform/ comes from

`platform/` is a copy of [Dinghy](https://github.com/zvenson/dinghy) by zvenson, the minimal firmware for the M-Vave FM-1, itself based on [Felucca](https://github.com/hugelton/Felucca) (Leo Kuroshita, Hügelton Instruments) and [SLOOP](https://github.com/isod89/sloop-fm1) (isod89).

| | |
| --- | --- |
| Version | Dinghy 0.4, commit `22acb9165ebc8baa712af3aff8c507e230c1e028` (23 October 2026) |
| Copied on | 24 October 2026 |
| License | GPL-3.0-only (`LICENSE`, credits in `LICENSING.md`) |

## What changed from the original

- Removed `assets/logo.svg`, `assets/logo.png` and `docs/screen.png`: the logo is by designburg, all rights reserved, and marks Dinghy, not its derivatives.
- Removed `.github/`: our workflow is `.github/workflows/ci.yml` at the root.
- Nothing else. Our build script is `tools/build.py` at the root, adapted from `platform/tools/build.py` (which stays here as it was, as a reference and for the sample app `firmware/src/dinghy.c`).

To update: copy the new version of Dinghy over this folder, repeat the exclusions above and update this table.
