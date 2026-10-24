# Diario AI

Le decisioni significative prese con Claude, dalla più recente.

| Data | Decisione | Perché |
| --- | --- | --- |
| 2026-10-24 | Passo 1 direttamente su `main`; dal passo 2 branch e pull request | Un repository vuoto non accetta pull request: il primo commit fa da base |
| 2026-10-24 | Moduli puri compilati uno per uno, solo `main.c` include `platform.c` | Testare la logica sul computer senza la piattaforma; la build controlla che i moduli non tocchino registri |
| 2026-10-24 | Identità del pacchetto `FM-1_95N` per Space Invaders | Libera nell'elenco di Dinghy; da confermare con il Firmware Hub prima del rilascio |
| 2026-10-24 | Monorepo nuovo, solo C, GPL-3.0; il TypeScript resta in insert-coin e qui entrano solo le tracce di riferimento | Da condividere con la community senza mescolare giochi web e firmware |
| 2026-10-24 | Tasti di Space Invaders e Galaga: OP1-OP3 sinistra, OP4-OP5 fuoco, OP6-PIT-GLO destra; Breakout con gli encoder SELECT e KNOB1 | Scelta di Luca: due mani sulla tastiera, encoder come paddle |
| 2026-10-24 | Schermo come strato riutilizzabile, configurato per gioco | Dopo Space Invaders (224 × 256) verranno Breakout (228 × 208) e Galaga (224 × 288) |
| 2026-10-24 | Simulatore nel browser come ultimo passo, ma obbligatorio | Provare senza installare il firmware sul synth |
| 2026-10-24 | Base Dinghy, porting in C con il TypeScript come specifica e verifica di parità | Il chip non ha FPU: JavaScript, traduzione automatica ed emulazione della ROM sono scartati ([analisi](https://claude.ai/artifact/SsyDLc6sKQEehsGhHX6bCZ)) |
