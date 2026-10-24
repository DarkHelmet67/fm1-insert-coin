# Linee guida per lo sviluppo (umani e AI)

Queste regole valgono per chiunque scriva codice in questo repository, persone o assistenti AI. Alcune sono verificate automaticamente (`make format-check`, i warning trattati come errori, i controlli di `tools/build.py`); le altre si controllano in revisione.

## AI for humans

Il codice di questo progetto sarà letto da persone: **code for humans, not for AI**.

- Usare pattern noti e C semplice, non soluzioni "furbe".
- Niente codice monolitico: file e funzioni piccoli, con una sola responsabilità. Un file che supera un paio di centinaia di righe va diviso.
- Tante piccole funzioni helper, facili da testare unitariamente.
- Commentare le parti più complesse spiegando il _perché_, non il _cosa_.

## Regole di sviluppo

| Regola | Come si applica | Controllo |
| --- | --- | --- |
| Logica pura, separata dall'hardware | Solo `main.c` di ogni gioco parla con l'FM-1 (include `platform.c`), aiutato dai file `libs/*/*_fm1.h` che include dopo `platform.c`. Tutti i file `.c` di `games/*/src/` e `libs/` sono C senza hardware, compilati a parte e testati sul computer | `tools/build.py` compila questi moduli uno per uno; i registri si toccano solo in `platform/firmware/hal/` |
| Stile funzionale | Lo stato si passa come `const T *` e una funzione restituisce il nuovo stato (`GameState game_step(const GameState *s, uint32_t actions)`); niente variabili globali nei moduli puri | Revisione |
| Solo numeri interi | Niente `float` né `double`, niente divisione a 64 bit: il chip non ha FPU né libgcc. Punto fisso (Q15, Q16) dove serve | Il link del firmware fallisce |
| Ogni funzione, struct ed enum ha un commento Doxygen | `/** ... */` con una riga che dice cosa fa; `@param`/`@returns` quando non sono ovvi | Revisione |
| Warning zero | `-std=c99 -Wall -Wextra -Werror` per i test sul computer | `make test` |
| Formattazione | clang-format con `.clang-format` | `make format-check` |
| Valori incerti in un solo file | Tutto ciò che è una scelta e non un fatto dell'originale (tasti, colori, tempi stimati) sta in `games/<gioco>/src/config.h` | Revisione |

### Eccezioni consentite

- **`main.c` è il guscio imperativo.** Il ciclo principale, lo schermo, i LED e l'interrupt audio tengono stato in variabili `static`: è il confine con l'hardware, come il game loop della versione web.
- **I file `*_fm1.h`.** Il codice di collegamento riusato da più giochi (per esempio l'invio dello schermo al display) sta in `libs/<modulo>/<nome>_fm1.h`, con funzioni `static`: si include solo da `main.c`, dopo `platform.c`, e non si compila da solo.
- **Il framebuffer.** Disegnare significa scrivere pixel in un buffer: le funzioni di disegno ricevono il buffer da modificare, e solo quello.
- **`platform/` è di Dinghy.** Si copia com'è dal progetto originale e non segue le nostre regole né il nostro stile; ogni modifica va annotata in `platform/UPSTREAM.md`.
- **Test.** Nei file `*_test.c` le fixture mutabili sono ammesse.

Se una regola diventa un ostacolo reale, si discute e si aggiorna questo file.

## Struttura e convenzioni

- Codice e commenti in inglese, documentazione (`README.md`, `docs/`) in italiano.
- `platform/` è Dinghy, `libs/` il nostro codice condiviso tra i giochi, `games/<gioco>/` un gioco con il suo `firmware.json` (identità del pacchetto, nome sulla schermata di aggiornamento).
- Il codice condiviso in `libs/` non include mai da `games/`.
- Ogni gioco è il porting fedele della versione web di [insert-coin](https://github.com/DarkHelmet67/insert-coin): la logica in TypeScript è la specifica, e la parità si verifica con tracce registrate fotogramma per fotogramma. Il TypeScript non entra in questo repository: entrano solo le tracce, come file di test.
- Ogni nuovo modulo arriva con i suoi test (`modulo_test.c` accanto a `modulo.c`) e con la sua guida in `docs/`.
- Ogni decisione significativa presa con l'AI va nel [diario AI](docs/ai-workflow.md).
- Ogni modifica significativa va riportata nel [prompt unico](docs/prompt-unico.md), che deve sempre poter ricreare il progetto com'è.

## Licenza

Tutto il repository è GPL-3.0, come Dinghy da cui deriva: chi riceve un `.fwsc` ha diritto al sorgente. Ogni file C nuovo inizia con `/* SPDX-License-Identifier: GPL-3.0-only */`. I crediti stanno in [LICENSING.md](LICENSING.md).

## Prima di ogni commit

```bash
make format-check && make test && make host GAME=<gioco>
```

Il firmware (`python3 tools/build.py <gioco>`) richiede la toolchain JieLi; lo costruisce comunque GitHub Actions a ogni push.
