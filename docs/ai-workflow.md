# Diario AI

Le decisioni significative prese con Claude, dalla più recente.

| Data | Decisione | Perché |
| --- | --- | --- |
| 2026-10-24 | Sintetizzatore a interi condiviso (`libs/audio`) con gli stessi dati di `sound-bank.ts`; curve esponenziali in ottave con tabelle di 2^x e log2 | Il chip non ha FPU; i suoni del web sono già pochi numeri, basta suonarli allo stesso modo |
| 2026-10-24 | Confronto misurato: il vero `synth.ts` in un `OfflineAudioContext` di Chromium contro il C, inviluppo, volume e altezza (`make sounds`) | "Suona uguale" diventa un numero; ha portato alle correzioni del livello dell'onda quadra (−1,5 dB) e del filtro (poli a 1,5 × il taglio) |
| 2026-10-24 | Coda a 16 posti fra ciclo principale e interrupt audio; record salvato solo a suoni finiti | Niente interrupt da disattivare; la cancellazione della flash ferma anche l'audio |
| 2026-10-24 | Ogni fotogramma ridisegnato da zero in un secondo framebuffer, poi `fb_present` copia solo le righe cambiate | Lo stesso disegno semplice del canvas web; al display vanno solo le righe diverse |
| 2026-10-24 | PLAY per moneta e partenza, FX per colori e bianco e nero; legende dell'attract coi tasti del synth, più in alto di 8 pixel | Pulsanti con LED vicini alla tastiera; la riga 240 non arriva sul display |
| 2026-10-24 | Record nella flash con `settings_save`, salvato solo dopo la partita, `STORE_MAGIC` "INVD" | Cancellare la flash ferma il gioco per 50 ms; la magia propria evita di leggere i dati di altri firmware |
| 2026-10-24 | Tabelle di `config.h` spostate in `config.c` | Includere `config.h` nei moduli di disegno senza avvisi di costanti inutilizzate |
| 2026-10-24 | Logica di Space Invaders in C con gli stessi moduli e nomi del TypeScript; struct per `const *`, restituite per valore; array fissi | Chi conosce insert-coin si ritrova; lo stile funzionale resta senza allocazioni. Parità completa sulle tre tracce al primo tentativo |
| 2026-10-24 | Sprite come stringhe ASCII (`SPRITE_DEFINE`), convertite in bitmap a 32 bit per riga solo per le collisioni (`libs/sprite`) | Disegni leggibili come nel TypeScript; la conversione costa poco e la libreria serve anche a Galaga e Breakout |
| 2026-10-24 | Nessuna modalità debug nel porting | Serve a provare i round avanzati nel browser; sul synth complicherebbe senza aiutare |
| 2026-10-24 | Tracce: riga per fotogramma con comandi e hash FNV-1a della stringa di stato; tre partite (idle, restless, aiming) | La logica web è deterministica: stessi comandi, stessa partita; l'hash tiene i file piccoli (1 MB) |
| 2026-10-24 | Registratore in Node fuori da insert-coin, che carica il TypeScript con il Vite di insert-coin | Il TypeScript resta in insert-coin; Vite serve perché il gioco legge `import.meta.env` |
| 2026-10-24 | Giocatore automatico con schivata (bomba sotto riga 120, entro 18 pixel) | Fra cinque prove è l'unico che arriva al secondo round |
| 2026-10-24 | Schermo: framebuffer a indici alla risoluzione originale, mappa riga per riga verso il display, invio delle sole righe cambiate con due buffer | Un'immagine intera costa 31 ms, un fotogramma ne ha 16,7; la mappa per righe copre centratura, taglio e salto di righe con un solo meccanismo |
| 2026-10-24 | Space Invaders a pixel 1:1: righe 8-239 e 243-250 | Le 8 righe in alto e le 3 sotto la linea verde sono vuote nell'originale |
| 2026-10-24 | Modalità host su macOS: il `Makefile` rinomina la sezione `.pool` | Mach-O rifiuta i nomi di sezione senza segmento, e `platform/` non si tocca |
| 2026-10-24 | Passo 1 direttamente su `main`; dal passo 2 branch e pull request | Un repository vuoto non accetta pull request: il primo commit fa da base |
| 2026-10-24 | Moduli puri compilati uno per uno, solo `main.c` include `platform.c` | Testare la logica sul computer senza la piattaforma; la build controlla che i moduli non tocchino registri |
| 2026-10-24 | Identità del pacchetto `FM-1_95N` per Space Invaders | Libera nell'elenco di Dinghy; da confermare con il Firmware Hub prima del rilascio |
| 2026-10-24 | Monorepo nuovo, solo C, GPL-3.0; il TypeScript resta in insert-coin e qui entrano solo le tracce di riferimento | Da condividere con la community senza mescolare giochi web e firmware |
| 2026-10-24 | Tasti di Space Invaders e Galaga: OP1-OP3 sinistra, OP4-OP5 fuoco, OP6-PIT-GLO destra; Breakout con gli encoder SELECT e KNOB1 | Scelta di Luca: due mani sulla tastiera, encoder come paddle |
| 2026-10-24 | Schermo come strato riutilizzabile, configurato per gioco | Dopo Space Invaders (224 × 256) verranno Breakout (228 × 208) e Galaga (224 × 288) |
| 2026-10-24 | Simulatore nel browser come ultimo passo, ma obbligatorio | Provare senza installare il firmware sul synth |
| 2026-10-24 | Base Dinghy, porting in C con il TypeScript come specifica e verifica di parità | Il chip non ha FPU: JavaScript, traduzione automatica ed emulazione della ROM sono scartati ([analisi](https://claude.ai/artifact/SsyDLc6sKQEehsGhHX6bCZ)) |
