# 04 · La logica di gioco in C

Il passo più lungo del porting: riscrivere in C la logica di Space Invaders della versione web, modulo per modulo, finché il gioco in C non produce **esattamente** le stesse partite. Il risultato si misura con le tracce della [guida 03](03-tracce.md): 32.000 fotogrammi in tre partite, e in ognuno lo stato del C ha lo stesso hash di quello del TypeScript.

## Da TypeScript a C, file per file

La struttura resta quella di insert-coin, così chi conosce un repository si ritrova nell'altro:

| TypeScript (insert-coin) | C (`games/space-invaders/src/`) | Cosa fa |
| --- | --- | --- |
| `sprites.ts` | `sprites.h/.c` | I disegni, una stringa per riga di pixel, identici |
| `aliens.ts` | `aliens.h/.c` | La formazione iniziale, larghezze, punti, altezze dei round |
| `fleet.ts` | `fleet.h/.c` | La marcia: un invasore per fotogramma, discesa, inversione |
| `cannon.ts`, `shot.ts` | `cannon.h/.c` | Il cannone e il suo colpo |
| `bombs.ts` | `bombs.h/.c` | Le tre bombe a turno, le tabelle delle colonne, la ricarica |
| `ufo.ts` | `ufo.h/.c` | Il disco volante e il trucco del 23° colpo |
| `shields.ts` | `shields.h/.c` | Gli scudi che si sbriciolano |
| `effects.ts` | `effects.h/.c` | Esplosioni e schizzi di breve durata |
| `collisions.ts` | `collisions.h/.c` | Le regole di collisione, una funzione ciascuna |
| `playing.ts` | `playing.h/.c` | Un fotogramma di partita: movimento, poi le regole in ordine |
| `game.ts` | `game.h/.c` | Attract, partita, game over, record |
| `controls.ts` | `controls.h` | Direzione, fuoco, moneta: cosa fa il giocatore in un fotogramma |

Restano fuori il disegno, i suoni e la modalità debug: il disegno e i suoni arrivano ai passi 5 e 6, la modalità debug non serve sul synth.

## Lo stesso stile, in C

La versione web è funzionale: ogni funzione riceve uno stato immutabile e ne restituisce uno nuovo. In C lo stile resta lo stesso, con un'idea semplice: **la struct si passa per puntatore costante e si restituisce per valore**.

```c
/* TypeScript: export const stepUfo = (state: UfoState, aliensLeft, shotsFired): UfoState => ... */
Ufo ufo_step(const Ufo *ufo, uint32_t aliens_left, uint32_t shots_fired)
{
    Ufo next = *ufo;              /* una copia locale */
    if (ufo->countdown > 0) {
        next.countdown--;         /* si modifica solo la copia */
        return next;
    }
    ...
}
```

Le liste immutabili del TypeScript (`readonly Alien[]`) diventano array a dimensione fissa con un contatore: 55 invasori, 3 bombe, 4 scudi, al massimo 24 effetti. Niente `malloc`: sul chip non c'è un allocatore e non serve.

Le regole di collisione si applicano come nel TypeScript, dove sono un array di funzioni passato a `reduce`:

```c
static const Rule COLLISION_RULES[] = {
    shot_hits_alien,  shot_hits_ufo,     shot_hits_bomb,   shot_hits_shield,
    bombs_hit_cannon, bombs_hit_shields, bombs_hit_ground, aliens_crush_shields,
    award_extra_life, next_round_if_cleared,
};
for (uint32_t i = 0; i < RULE_COUNT; i++)
    current = COLLISION_RULES[i](&current);
```

### Quanto costa copiare

Lo stato di una partita (`PlayingState`) occupa circa 1 KB: ogni regola ne fa una copia, una ventina di copie per fotogramma, cioè circa 20 KB di `memcpy` ogni 16,7 ms. Per un processore a 320 MHz è un'inezia. Lo stack del firmware è di 24 KB: misurato con `-fstack-usage`, il percorso più profondo (`game_step` → `playing_step` → una regola) ne usa circa 9.

## Le collisioni al pixel: `libs/sprite`

I disegni restano leggibili come nel TypeScript, una stringa per riga:

```c
SPRITE_DEFINE(SQUID_0, 8,
    "...XX...",
    "..XXXX..",
    ...);
```

Per le collisioni una stringa è scomoda, così `bitmap_from_sprite()` la converte in una `Bitmap`: una parola a 32 bit per riga, il bit 31 è il pixel più a sinistra. Su questa lavorano `placed_overlap()` (prima il rettangolo comune, poi i pixel uno per uno, come `bitmapsOverlap` di `@arcade/collision`) e `bitmap_erase()` (i buchi negli scudi). La libreria è condivisa: servirà anche a Galaga e Breakout.

## Le trappole della traduzione

Il codice è quasi una traduzione riga per riga, ma alcune differenze fra JavaScript e C sono trappole silenziose: il gioco funziona, solo non è più lo stesso. Le tracce le trovano tutte.

- **Divisione con numeri negativi.** `Math.floor(-5 / 2)` vale −3, in C `-5 / 2` vale −2. L'esplosione di un invasore stretto si centra con un numero negativo, per questo `effects.c` ha `floor_div()`.
- **Numeri non interi.** Il TypeScript muove cannone e colpo con velocità in pixel al secondo moltiplicate per 1/60. Verificato in Node: `60 × (1/60)` e `240 × (1/60)` danno esattamente 1 e 4, quindi le posizioni restano intere e in C bastano gli interi.
- **Lo stato "prima" e lo stato "dopo".** In `advanceWorld` le bombe e il disco volante vedono la formazione *prima* del passo di questo fotogramma, perché l'oggetto letterale del TypeScript legge `state` per ogni campo. Il C passa a `bombs_step` la flotta vecchia, non quella appena mossa.
- **Rimozioni per identità.** `removeBombs` del TypeScript filtra per riferimento; il C rimuove per indice, scorrendo dalla fine quando ne toglie più di una per non spostare gli indici ancora da usare.
- **Gli scudi colpiti da più bombe.** Nello stesso fotogramma tutte le bombe si controllano contro gli scudi com'erano prima della regola, poi i danni si applicano uno dopo l'altro: proprio come fa `reduce` nel TypeScript.
- **"Nessun valore".** `undefined` diventa un campo: `active` per il colpo, `flying` per il disco volante, `cannon_explosion` a 0 quando il cannone è vivo (nel TypeScript non scende mai sotto 1 prima di diventare `undefined`), `NO_POINTS` per gli effetti senza punteggio.

## Il test di parità

`parity_test.c` legge le tre tracce, rigioca i comandi di ogni fotogramma e confronta l'hash dello stato del C con quello registrato. `trace_state.c` scrive lo stato del C nella stessa stringa canonica di [`tools/traces/FORMAT.md`](../tools/traces/FORMAT.md).

```
$ make test
== build/host/games/space-invaders/src/parity_test
- idle_matches_the_web_version
- restless_matches_the_web_version
- aiming_matches_the_web_version
- the_text_never_overflows
4 checks, 0 failed
```

Alla prima differenza il test si ferma e stampa il fotogramma e la stringa di stato del C. Basta confrontarla con quella del TypeScript per trovare il campo sbagliato:

```bash
node tools/traces/record-space-invaders.mjs ../insert-coin --dump aiming 574
```

Per esempio, con una discesa di 7 pixel invece di 8 il test lo dice subito:

```
idle: frame 611 differs, trace 381ec416, C 0b827da5
P|hi=0|rec=0|r=1;s=0;l=3;xl=0;sf=0;c=16;ce=-;sh=-;fl=1:-1:1:10:4:o5.1@44:135:1,...
```

Il primo invasore è a y = 135 invece di 136. Il test è una rete di sicurezza per tutto ciò che verrà: se una modifica al C cambia anche un solo pixel di uno scudo in uno solo dei 32.000 fotogrammi, `make test` fallisce.

## I test unitari

Accanto alla parità, ogni modulo ha i suoi test, tradotti da quelli della versione web (`aliens_test.c`, `fleet_test.c`, `bombs_test.c`, `ufo_test.c`, `cannon_test.c`, `shields_test.c`, `effects_test.c`, `collisions_test.c`, `playing_test.c`, `game_test.c`). La parità dice *se* il gioco è lo stesso, i test unitari dicono *dove* si è rotto e spiegano come funziona ogni regola: `it_moves_the_last_invader_3_pixels_right_but_only_2_left` si legge come la documentazione di una stranezza dell'originale.

## Il prompt per farlo con un'AI

> Traduci in C99 i moduli di logica di `games/space-invaders/src/` di insert-coin, uno per file, con lo stesso nome. Stato passato come `const T *` e restituito per valore, array fissi al posto delle liste, solo interi. Poi scrivi `trace_state.c` che produce la stringa di `tools/traces/FORMAT.md` e un test che rigioca le tracce e confronta gli hash fotogramma per fotogramma. Se un fotogramma differisce, confronta la stringa del C con quella di `--dump` e correggi il modulo, non la traccia.

Con l'approccio "traduci, poi verifica con le tracce", la parità completa è arrivata al primo tentativo: la traduzione fedele del TypeScript, con le trappole qui sopra in mente, è bastata.
