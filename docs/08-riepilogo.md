# 08 · Riepilogo e prossimi giochi

Space Invaders gira sull'FM-1: stessa logica della versione web dimostrata su 32.000 fotogrammi, grafica a pixel 1:1, i suoni dello stesso sintetizzatore misurati contro Web Audio, il record nella flash, e un simulatore nel browser che esegue lo stesso codice. Questa guida mette insieme i pezzi e spiega come aggiungere il prossimo gioco.

## Come si tiene insieme

```
            tasti, pulsanti                                   schermo SPI, altoparlante
                  │                                                        ▲
  ┌───────────────┴──────────────── main.c (FM-1) / sim.c (browser) ───────┴───────────────┐
  │                                                                                         │
  │   app_step ── input_controls ── game_step ── sounds_for          render_game            │
  │                                    │                                  │                  │
  │                     playing_step ──┼── fleet, bombs, ufo,          draw, font            │
  │                                    │   shields, collisions        (libs/screen, font)    │
  │                                    │                                  │                  │
  │                                    └── parità con il TypeScript   fb_present → screen_push│
  │                                        (tracce, parity_test)      mixer → audio_render   │
  └─────────────────────────────────────────────────────────────────────────────────────────┘
```

- **Logica pura** (`games/space-invaders/src/*.c`): ogni modulo riceve lo stato come `const T *` e restituisce lo stato nuovo. Si prova sul computer con `make test`, che rigioca anche le tre tracce registrate dalla versione web.
- **Librerie condivise** (`libs/`): schermo, disegno, font, sprite e collisioni, comandi, orologio a 60 Hz, sintetizzatore, record, tracce. Nessuna conosce Space Invaders.
- **Due gusci**: `main.c` per il synth (Dinghy in `platform/`), `sim/sim.c` per il browser. Tutto il resto è comune.

## Cosa è verificato, e come

| Cosa | Verifica |
| --- | --- |
| La logica è quella della versione web | `parity_test`: tre partite, 32.000 fotogrammi, ogni stato con lo stesso hash |
| Ogni modulo fa ciò che dice | 600+ controlli in `make test`, tradotti dai test di insert-coin |
| La grafica | `render_test` e le schermate della modalità host (artefatto `host-screens` in CI) |
| I suoni | `make sounds`: inviluppo, volume e altezza contro il vero `synth.ts` in Chromium |
| Il firmware si costruisce | Lavoro `firmware` in CI, con la toolchain JieLi (artefatto `firmware`) |
| Il simulatore funziona | `tools/sim/smoke.mjs` in CI, `tools/sim/test.mjs` con Playwright in locale |
| L'installer web parla il protocollo | `make installer-test` in CI con un FM-1 finto, `tools/installer/page-test.mjs` con Playwright in locale ([guida 09](09-installer.md)) |
| Sul synth vero | Da provare: installare il `.fwsc` dell'artefatto `firmware`, o dall'installer web dopo il merge |

Sul synth restano da confermare le cose che nessun test sul computer può dire: la fluidità a 60 Hz con il display reale, la luminosità dei LED, il volume dell'altoparlante. L'identità del pacchetto (`FM-1_95`) va concordata con il FM-1 Firmware Hub prima di pubblicarlo lì.

## Aggiungere un gioco

Galaga e Breakout seguiranno la stessa strada; ogni passo riusa ciò che c'è già.

1. **La cartella.** `games/<gioco>/` con `firmware.json` (identità `FM-1_9N` libera, nome e colore della schermata di aggiornamento), `src/config.h` e `src/config.c` per i valori che sono scelte (tasti, colori, righe mostrate).
2. **Le tracce.** Un registratore come `tools/traces/record-space-invaders.mjs` che carica la versione web e scrive le partite in `games/<gioco>/traces/`; un `trace_state.c` che scrive lo stato del C nella stessa forma.
3. **La logica.** Un modulo C per ogni file di logica TypeScript, con i suoi test; `parity_test.c` deve diventare verde.
4. **Schermo e comandi.** `screen_map_centered()` o `screen_map_rows()` decide come la risoluzione del gioco sta nei 240 × 240 (Breakout 228 × 208 entra centrato; Galaga 224 × 288 va tagliato o compresso: è la decisione aperta). Breakout legge gli encoder SELECT e KNOB1 con `fm1_enc_take()`.
5. **Suoni.** I numeri del suo `sound-bank.ts` in `sounds.c`; Galaga usa il chip sonoro WSG di Namco (`@arcade/wsg` nella versione web), che servirà portare in `libs/audio`.
6. **`app.c`, `main.c`, `sim/sim.c`.** Copiati da Space Invaders e adattati: sono gusci sottili.
7. **CI.** Niente da fare: i lavori girano su ogni `games/*/firmware.json` e `games/*/sim/sim.c`, e il gioco compare da solo nell'installer web.
