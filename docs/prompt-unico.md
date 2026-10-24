# Prompt unico

Il prompt che ricrea questo repository com'è oggi. Va aggiornato a ogni modifica significativa.

---

Crea il monorepo **fm1-insert-coin**: giochi arcade degli anni '80 come firmware per il sintetizzatore M-Vave FM-1 (JieLi AC791N, niente FPU, schermo 240 × 240 RGB565 su SPI, 27 tasti, 14 pulsanti, 7 encoder). Licenza GPL-3.0-only. Codice e commenti in inglese, documentazione in italiano.

**Piattaforma.** Copia [Dinghy](https://github.com/zvenson/dinghy) 0.4 (commit `22acb91`) in `platform/` senza il logo di designburg, `docs/screen.png` e `.github/`; annota provenienza e modifiche in `platform/UPSTREAM.md`. Non modificare `platform/`.

**Struttura.** `libs/` (codice condiviso, C puro), `games/<gioco>/` con `firmware.json` (`identity` FM-1_9N, `screen_name`, `screen_colour` RGB565 in esadecimale), `src/main.c` (unico file che include `platform.c` e definisce `app_main()` e `app_audio()`), `src/config.h` (i valori che sono scelte), `scripts/*.txt` (script per la modalità host). `tools/build.py GAME`: adattamento di `platform/tools/build.py` che compila `main.c` più ogni altro `.c` del gioco e di `libs/` come oggetto separato (esclusi `*_test.c`), con i controlli di Dinghy estesi ai nostri file; solo FM-1. `tools/ppm_to_png.py`: screenshot della modalità host in PNG 480 × 480.

**Regole** (in `CLAUDE.md`): code for humans; file piccoli; logica pura con stato passato come `const T *` e nuovo stato restituito; solo interi; commenti Doxygen ovunque; `-std=c99 -Wall -Wextra -Werror`; clang-format (LLVM, indentazione 4, colonne 100, graffe Linux); valori incerti in `config.h`; test `modulo_test.c` accanto al modulo con l'harness `libs/test/check.h` (`CHECK`, `CHECK_EQ`, `RUN`, `check_report`).

**Makefile.** `make test` (compila ed esegue ogni `*_test.c` con i `.c` della sua cartella), `make host GAME=… [SCRIPT=…]` (il gioco con `-DDINGHY_HOST`, poi gli screenshot in PNG), `make format`, `make format-check`.

**CI** (`.github/workflows/ci.yml`, a ogni push e PR): lavoro `host` (format-check, test, ogni gioco in modalità host, screenshot come artefatto) e lavoro `firmware` (toolchain JieLi da pkgman.jieliapp.com, SDK AC79 `AC79NN_SDK_V1.2.1_2023-12-13` da gitee.com, `tools/build.py` per ogni gioco, `.fwsc` come artefatto).

**libs/input.** `fm1_keys.h`: i tasti per posizione e le etichette dei tasti neri (OP1 = 1, OP2 = 3, OP3 = 5, OP4 = 8, OP5 = 10, OP6 = 13, PIT = 15, GLO = 17, MONO = 20, POLY = 22, dalla tabella `edit_keys` di fm1_open). `keymap.h/.c`: `KeyMap {left, right, fire}` come maschere di `fm1_in.notes`; `keymap_actions()` restituisce i bit `ACTION_LEFT`, `ACTION_RIGHT`, `ACTION_FIRE`, con sinistra e destra insieme che si annullano; `keymap_keys()` tutti i tasti usati.

**libs/screen e libs/clock.** `framebuffer.h/.c`: `Framebuffer` (al massimo 240 × 288, un byte per pixel con indice di colore su 16, un bit "cambiata" per riga), `fb_init`, `fb_get`, `fb_set` (segna la riga solo se il pixel cambia), `fb_fill_rect`, `fb_row_dirty`, `fb_mark_all_dirty`, `fb_clear_dirty`, macro `RGB565`. `screen_map.h/.c`: `ScreenMap` (per ognuna delle 240 righe del display la riga del gioco o `SCREEN_NO_ROW`, colonne centrate 1:1), `screen_map_centered(w, h)`, `screen_map_rows(w, RowRange[], n)` (serie di righe impilate e centrate). `screen_push.h/.c`: `screen_next_band()` (prossima serie di righe cambiate, al massimo N) e `screen_render_row()` (una riga colorata con la tavolozza). `screen_fm1.h` (incluso da `main.c` dopo `platform.c`): due buffer da 240 × 32 nel pool, `screen_palette()` (byte scambiati per il display), `screen_clear()`, `screen_push()`. `frame_clock.h/.c`: `frame_clock_frames(ms)` = ms × 60 / 1000 senza overflow né divisione a 64 bit. Su macOS il `Makefile` aggiunge `-D'section(x)=section("__DATA,__pool")'` alla build host.

**Space Invaders, passo 1.** Identità `FM-1_95`, schermata di aggiornamento "SPACE INVADERS" in verde `0x07E0`. Tasti: OP1-OP3 sinistra, OP4-OP5 fuoco, OP6-PIT-GLO destra. `main.c`: titolo "SPACE / INVADERS" in FONT_L bianco, "FM-1 EDITION" in verde, legenda "OP1-3  OP4-5  OP6-GLO" in basso; i tasti di gioco accesi deboli e pieni quando premuti; una riga mostra le azioni (`<`, `FIRE`, `>`). Audio muto.

**Space Invaders, passo 2.** `config.h` aggiunge `GAME_WIDTH` 224, `GAME_HEIGHT` 256, `SHOWN_ROWS` {8-239, 243-250}, gli indici di colore (sfondo, testo, cannone, UFO, colpi, tre righe di alieni) e la tavolozza di colors.config.ts. `main.c` mostra un cartellino di prova (bordi, linea verde a riga 239, tre cannoni di riserva a riga 243, cinque barre colorate) e un blocco 13 × 8 a riga 216 che i tasti muovono di un pixel per fotogramma (bianco col fuoco), con il ciclo a 60 Hz di `frame_clock` (al massimo 4 fotogrammi di recupero) e `screen_push()` a ogni giro. Script host `scripts/demo.txt`.

**Documentazione.** README per la community (giochi, installazione con avvertenze e ritorno al firmware V15, modalità host, build, struttura, guide), `LICENSING.md`, guide `docs/00-introduzione.md` e `docs/01-piattaforma.md`, `docs/ai-workflow.md`, questo prompt.
