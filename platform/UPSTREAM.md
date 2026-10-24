# Da dove viene platform/

`platform/` è una copia di [Dinghy](https://github.com/zvenson/dinghy) di zvenson, il firmware minimo per l'M-Vave FM-1, a sua volta basato su [Felucca](https://github.com/hugelton/Felucca) (Leo Kuroshita, Hügelton Instruments) e [SLOOP](https://github.com/isod89/sloop-fm1) (isod89).

| | |
| --- | --- |
| Versione | Dinghy 0.4, commit `22acb9165ebc8baa712af3aff8c507e230c1e028` (23 ottobre 2026) |
| Copiato il | 24 ottobre 2026 |
| Licenza | GPL-3.0-only (`LICENSE`, crediti in `LICENSING.md`) |

## Cosa è cambiato rispetto all'originale

- Tolti `assets/logo.svg`, `assets/logo.png` e `docs/screen.png`: il logo è di designburg, tutti i diritti riservati, e segna Dinghy, non i suoi derivati.
- Tolto `.github/`: il nostro workflow è in `.github/workflows/ci.yml` alla radice.
- Nient'altro. Il nostro script di build è `tools/build.py` alla radice, adattato da `platform/tools/build.py` (che resta qui com'era, come riferimento e per l'app di esempio `firmware/src/dinghy.c`).

Per aggiornare: copiare la nuova versione di Dinghy sopra questa cartella, ripetere le esclusioni qui sopra e aggiornare questa tabella.
