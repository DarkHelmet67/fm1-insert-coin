# Licenza e crediti

fm1-insert-coin è software libero sotto la GNU General Public License, versione 3 soltanto (`GPL-3.0-only`, testo completo in [LICENSE](LICENSE)). Si può usare, studiare, modificare e condividere; chi distribuisce un firmware costruito da qui deve dare anche il sorgente completo, con la stessa licenza.

## Chi ha fatto cosa

| Parte | Autori |
| --- | --- |
| `platform/`: la piattaforma del firmware (avvio, USB, aggiornamento, MIDI, audio, schermo, flash, modalità host) | [Dinghy](https://github.com/zvenson/dinghy) di zvenson, su [Felucca](https://github.com/hugelton/Felucca) di Leo Kuroshita (Hügelton Instruments) con le modifiche di [SLOOP](https://github.com/isod89/sloop-fm1) (isod89) e il livello SMK-37 di M. Geisler (genkigenki); dettagli in [platform/LICENSING.md](platform/LICENSING.md) |
| `tools/build.py` | Adattato da `platform/tools/build.py` (Leo Kuroshita, zvenson) |
| `installer/vendor/`: il protocollo di aggiornamento USB-MIDI e i pacchetti `.fwsc` per il browser | Leo Kuroshita (Hügelton Instruments), copiati da [Skiff](https://github.com/illuminus5/skiff); dettagli in [installer/UPSTREAM.md](installer/UPSTREAM.md) |
| `libs/`, `games/`, `docs/`, il resto | Luca (DarkHelmet67), con Claude |

I giochi sono porting della versione web di [insert-coin](https://github.com/DarkHelmet67/insert-coin) (MIT, stesso autore): disegni, suoni e regole sono ricostruiti dalle ricerche su hardware e software originali, senza ROM.

## Materiale di terzi

| Cosa | Licenza | Dove |
| --- | --- | --- |
| Font Terminus 8x16 in `platform/firmware/gen/felucca_font.h` | SIL OFL 1.1 | `platform/assets/fonts/Terminus-LICENSE.txt` |
| JieLi AC79 SDK: tre file letti al momento della build e messi nel pacchetto, nessuno in questo repository | Apache-2.0 | <https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK> |

## Marchi

"Space Invaders" è un marchio di Taito; "Galaga" e "Pac-Man" di Bandai Namco; "Breakout" di Atari. "M-VAVE" e "FM-1" appartengono ai rispettivi proprietari. Questo progetto amatoriale e didattico non è affiliato né approvato da nessuno di loro.
