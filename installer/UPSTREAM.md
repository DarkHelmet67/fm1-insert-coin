# Where installer/vendor/ comes from

`vendor/fm1ota.js` (the USB-MIDI update protocol) and `vendor/fm1pkg.js` (the `.fwsc` packages) are by Leo Kuroshita (@kurogedelic, Hügelton Instruments), author of [Felucca](https://github.com/hugelton/Felucca), copied from [Skiff](https://github.com/illuminus5/skiff), which uses them in its web installer.

| | |
| --- | --- |
| From | Skiff's `web/fm1ota.js` and `web/fm1pkg.js`, commit `b03d98acd8c6356fb84e8edf2268f374a89eb8a5` |
| Copied on | 25 October 2026 |
| License | GPL-3.0-only, like this repository |

Like `platform/`, they follow neither our rules nor our style.

## What changed from the original

- `fm1ota.js`, `Updater.find()`: the MIDI output is paired with the input when it has the same name **or contains it** (the rule of [SLOOP](https://github.com/isod89/sloop-fm1)'s installer). On Windows the output of the input "Felucca" is called "MIDIOUT2 (Felucca)", and Skiff's copy did not find it. The line is marked `fm1-insert-coin:`.
- Nothing else. `fm1pkg.js` is unchanged; its functions for going back to the official firmware (`validateStockPackage`, `buildFeluccaPackage`) are not used by our page.

To update: copy the two files from the new version of Skiff, redo the change above, update the table and run `make installer-test`.
