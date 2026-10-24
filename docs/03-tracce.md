# 03 Le tracce di riferimento

Come si dimostra che il porting in C è lo stesso gioco della versione web? Guardarlo non basta: un invasore che spara un fotogramma più tardi, o un punteggio del disco volante sbagliato, sfuggono all'occhio. Il terzo passo registra dalla versione TypeScript tre partite complete, fotogramma per fotogramma, in un formato che il C potrà riprodurre e confrontare.

## L'idea

La logica della versione web è una funzione pura: `updateGame(stato, comandi)` restituisce lo stato del fotogramma dopo. Non c'è niente di casuale: le bombe seguono tabelle fisse, il disco volante un contatore. Quindi gli stessi comandi producono sempre la stessa partita. Basta registrare i comandi e lo stato di ogni fotogramma: il C riceverà gli stessi comandi e dovrà produrre gli stessi stati.

Lo stato intero di un fotogramma è grande (55 invasori, quattro scudi da 22 × 16 pixel), quindi la traccia non lo salva per esteso. Lo scrive come una stringa di testo con regole precise e ne salva un'impronta di 32 bit, l'hash FNV-1a:

```
1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18
```

Fotogramma 1234, comandi "destra e fuoco", schermata di gioco, impronta `0b4bf1d3`; i numeri in fondo sono solo per chi legge. Le regole della stringa sono in [tools/traces/FORMAT.md](../tools/traces/FORMAT.md), la specifica che il registratore e il C condividono.

## Il registratore

`tools/traces/record-space-invaders.mjs` è uno script Node che non contiene il gioco: lo carica da una copia di insert-coin attraverso il Vite di insert-coin stesso, che compila il TypeScript e imposta `import.meta.env` come nel browser. Il TypeScript resta dov'è, qui entrano solo le tracce.

```sh
git clone https://github.com/DarkHelmet67/insert-coin ../insert-coin
(cd ../insert-coin && pnpm install)
make traces INSERT_COIN=../insert-coin
```

Ogni traccia nomina nell'intestazione il commit di insert-coin da cui viene. Se la versione web cambia, si registrano di nuovo.

Tre partite, tutte con la moneta al fotogramma 60:

| Traccia | Fotogrammi | Cosa copre |
| --- | --- | --- |
| `idle` | 6000 | Nessuno gioca: le bombe distruggono i tre cannoni, game over, ritorno all'attract |
| `restless` | 6000 | Avanti e indietro ogni 90 fotogrammi, un colpo ogni 20 |
| `aiming` | 20000 | Un giocatore automatico che mira e schiva: arriva al secondo round e passa 1500 punti (cannone extra) |

Il giocatore automatico decide guardando lo stato, ma la traccia salva le sue scelte come comandi: il C le rigioca senza bisogno del giocatore. Per regolarlo ho provato cinque combinazioni di "quanto vicino" deve essere una bomba per scappare; la migliore (bomba sotto la riga 120 e a meno di 18 pixel) arriva al secondo round.

## Per il debug

Quando il C sbaglierà un fotogramma, l'impronta non dirà cosa. Il registratore stampa la stringa intera di qualunque fotogramma:

```sh
node tools/traces/record-space-invaders.mjs ../insert-coin --dump aiming 1234
```

e il C stamperà la sua: il primo carattere diverso indica il campo sbagliato.

## Il lato C

`libs/trace/` contiene ciò che serve al confronto, in C puro e testato:

- `text.c`: un buffer di testo a dimensione fissa per costruire la stringa di stato senza la libreria C, e `text_fnv1a()`;
- `trace_line.c`: la lettura di una riga di traccia.

Il primo test incrocia i due linguaggi: l'impronta di `A|hi=0`, la schermata attract prima di ogni partita, calcolata in C, deve essere `e8c203ae`, la stessa che il registratore TypeScript scrive in testa a ogni traccia.

Il confronto vero arriverà con la logica, al passo 4: una partita intera in C, fotogramma per fotogramma, contro le tre tracce.
