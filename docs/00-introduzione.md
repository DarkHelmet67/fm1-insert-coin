# 00 Introduzione

Questo repository porta i giochi di [insert-coin](https://github.com/DarkHelmet67/insert-coin), remake web dei classici arcade, su un hardware vero: il sintetizzatore tascabile M-Vave FM-1. È il seguito del progetto: stessa fedeltà all'originale, stesso metodo, ma su un chip senza browser, senza JavaScript e senza virgola mobile.

## Perché un sintetizzatore

Nel 2026 M-Vave ha aperto il firmware dell'FM-1 ([fm1_open](https://github.com/m-vave/fm1_open)) e in poche settimane la community ha scritto più di cento firmware alternativi. Alcuni sono giochi: [FM-1 Doom](https://github.com/zvenson/fm1doom) gira un livello di Freedoom, [picovave](https://github.com/acidsound/picovave) esegue le cartucce PICO-8. L'FM-1 ha tutto quello che serve a un arcade tascabile:

| | |
| --- | --- |
| Processore | JieLi AC791N, due core, senza unità a virgola mobile |
| Memoria | 1 MiB di flash, circa 430 KB di RAM |
| Schermo | 240 × 240 a colori, su bus SPI |
| Comandi | 27 tasti con LED, 14 pulsanti, 7 encoder |
| Suono | Altoparlante, uscita cuffie, 44,1 kHz |

## Perché il C, e non il TypeScript

Il chip non ha FPU e in JavaScript ogni numero è a virgola mobile: un interprete JS dovrebbe emulare l'aritmetica in software, con poca RAM e nessuno che l'abbia mai provato su questo hardware. Tradurre il TypeScript in C in automatico non funziona con il nostro stile (oggetti immutabili, funzioni di ordine superiore). Emulare la scheda originale con la sua ROM non è distribuibile e contraddice il nostro approccio.

Resta la strada più onesta: riscrivere la logica in C, tenendo il TypeScript come **specifica eseguibile**. Il gioco web registra lo stato a ogni fotogramma con una sequenza di tasti fissa; il C, in modalità host, riceve gli stessi tasti e deve produrre lo stesso stato. Se le due tracce coincidono, è lo stesso gioco, e lo dimostra un test.

## Perché Dinghy

[Dinghy](https://github.com/zvenson/dinghy) è il firmware minimo dell'FM-1: un'onda sinusoidale sui tasti e, sotto, tutto ciò che serve (avvio, aggiornamento via USB, recupero, tasti, schermo, audio, memoria flash). Un'app scrive due funzioni, `app_main()` e `app_audio()`, e la modalità host le fa girare sul computer. Il suo autore lo ha usato per Doom, quindi è una base provata sul dispositivo. L'alternativa, fm1_open, è un sintetizzatore completo con un motore binario chiuso e una toolchain solo Windows.

## Il piano del primo gioco

Completato il 24 ottobre 2026; ogni passo ha la sua guida.

1. Il monorepo, Dinghy, la build host e la build del firmware in CI; il primo firmware con il titolo e i tasti accesi ([guida 01](01-piattaforma.md)).
2. Le librerie condivise: schermo e orologio a 60 Hz ([guida 02](02-schermo.md)).
3. Le tracce di riferimento registrate dalla versione TypeScript ([guida 03](03-tracce.md)).
4. La logica di Space Invaders in C, modulo per modulo, con la parità verde ([guida 04](04-logica.md)).
5. Disegno, comandi, attract, partita, record: il primo `.fwsc` giocabile ([guida 05](05-grafica-e-controlli.md)).
6. I suoni ([guida 06](06-suoni.md)).
7. Il simulatore nel browser ([guida 07](07-simulatore.md)).
8. La documentazione finale ([guida 08](08-riepilogo.md)).

Dopo il piano, chiesto da Luca guardando la pull request: l'installer nel browser, per installare senza riga di comando ([guida 09](09-installer.md)).

Le analisi e le decisioni prese prima di cominciare sono nel [diario AI](ai-workflow.md).
