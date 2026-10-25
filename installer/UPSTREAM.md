# Da dove viene installer/vendor/

`vendor/fm1ota.js` (il protocollo di aggiornamento USB-MIDI) e `vendor/fm1pkg.js` (i pacchetti `.fwsc`) sono di Leo Kuroshita (@kurogedelic, Hügelton Instruments), autore di [Felucca](https://github.com/hugelton/Felucca), copiati da [Skiff](https://github.com/illuminus5/skiff), che li usa nel suo installer web.

| | |
| --- | --- |
| Da | `web/fm1ota.js` e `web/fm1pkg.js` di Skiff, commit `b03d98acd8c6356fb84e8edf2268f374a89eb8a5` |
| Copiato il | 25 ottobre 2026 |
| Licenza | GPL-3.0-only, come questo repository |

Come `platform/`, non seguono le nostre regole né il nostro stile.

## Cosa è cambiato rispetto all'originale

- `fm1ota.js`, `Updater.find()`: l'uscita MIDI si abbina all'ingresso se ha lo stesso nome **o lo contiene** (regola dell'installer di [SLOOP](https://github.com/isod89/sloop-fm1)). Su Windows l'uscita dell'ingresso "Felucca" si chiama "MIDIOUT2 (Felucca)" e la copia di Skiff non la trovava. La riga è segnata con `fm1-insert-coin:`.
- Nient'altro. `fm1pkg.js` è identico; le sue funzioni per il ritorno al firmware ufficiale (`validateStockPackage`, `buildFeluccaPackage`) non sono usate dalla nostra pagina.

Per aggiornare: copiare i due file dalla nuova versione di Skiff, rifare la modifica qui sopra, aggiornare la tabella e lanciare `make installer-test`.
