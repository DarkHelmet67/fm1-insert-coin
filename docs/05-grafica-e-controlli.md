# 05 · Grafica, comandi e record

Con la logica identica alla versione web ([guida 04](04-logica.md)) manca tutto ciò che si vede e si tocca: disegnare il gioco, leggere i tasti del synth, le schermate, il record salvato. Alla fine di questo passo Space Invaders si gioca dall'inizio al game over.

<p align="center">
<img src="images/space-invaders-attract.png" width="240" alt="La schermata di attesa con la tabella dei punti">
<img src="images/space-invaders-partita.png" width="240" alt="Una partita: la formazione, gli scudi colpiti, il cannone">
<img src="images/space-invaders-mono.png" width="240" alt="La stessa partita in bianco e nero">
</p>

## Disegnare tutto, inviare poco

La versione web ridisegna l'intero canvas a ogni fotogramma: è il modo più semplice e il browser non fa una piega. Sul synth il problema è il display: un'immagine intera costa 31 ms di SPI, più di un fotogramma ([guida 02](02-schermo.md)). La soluzione tiene la semplicità del web:

1. `render_game()` disegna il fotogramma **da zero** in un framebuffer, `back`, come `renderGame` sul canvas;
2. `fb_present(&front, &back)` confronta le due immagini riga per riga e copia in `front` solo le righe diverse, segnandole come cambiate;
3. `screen_push()` invia al display solo quelle.

```c
render_game(&back, &game);
fb_present(&front, &back);
screen_push(&front, &map, palette);
```

In una partita cambiano poche righe per fotogramma: l'invasore che si è mosso, il colpo, le bombe, il cannone. Il confronto di 57.000 byte costa una frazione di millisecondo, il trasferimento risparmiato vale decine di millisecondi.

Il disegno ricalca i file `render-*.ts` della versione web, funzione per funzione (`render_header`, `render_ground_and_lives`, `render_playing`, `render_attract`), con le stesse coordinate. Due librerie nuove lo rendono possibile:

- `libs/screen/draw.h`: `fb_draw_sprite()`, `fb_draw_bitmap()` (gli scudi danneggiati) e `fb_clear()`;
- `libs/font`: il font 5 × 7 di insert-coin (`ARCADE_FONT`), copiato carattere per carattere, con `fb_draw_text()` e `fb_draw_text_centered()`.

Entrambe servono anche a Galaga e Breakout.

## Colori e bianco e nero

Il framebuffer contiene indici di colore, non colori: la tavolozza si applica solo all'invio. Per passare dal film colorato al monitor in bianco e nero (il tasto V del web, **FX** sul synth) basta cambiare tavolozza e rimandare tutte le righe: il gioco non ridisegna niente. Le due tavolozze, `COLORS` e `MONO_COLORS`, sono in `config.c` con gli stessi valori di `colors.config.ts`.

## I comandi

| Comando | Tasto dell'FM-1 | Come sul web |
| --- | --- | --- |
| Sinistra | OP1, OP2, OP3 | Freccia sinistra, A |
| Fuoco | OP4, OP5 | Spazio |
| Destra | OP6, PIT, GLO | Freccia destra, D |
| Moneta e partenza | PLAY | C, 5 |
| Colori / bianco e nero | FX | V |

`input_controls()` traduce lo stato del synth nei `Controls` della logica, con le stesse regole del web: il movimento segue i tasti tenuti premuti (sinistra e destra insieme si annullano), il fuoco e la moneta sono **pressioni**: tenere premuto OP4 non spara a raffica. La funzione è pura e testata: riceve i tasti di adesso e quelli del fotogramma prima.

I LED aiutano a trovare i tasti: quelli di gioco sono accesi deboli e si illuminano quando li premi; PLAY lampeggia finché si può iniziare una partita.

La scelta dei tasti e dei pulsanti sta in `config.h` (`CONTROLS`, `COIN_BUTTON`, `COLOR_BUTTON`): chi preferisce altri tasti cambia una riga.

## La schermata di attesa

Uguale a quella del web, con un adattamento: le ultime due righe nominano i tasti del synth invece di quelli del computer, e salgono di qualche pixel, perché la riga 240 dell'originale non arriva sul display (le righe mostrate sono 8-239 e 243-250). "PRESS PLAY" lampeggia come "INSERT COIN": acceso per mezzo secondo, spento per l'altro mezzo.

## Il record

La versione web salva il record nel browser con `@arcade/storage`; qui va nella flash del synth, con `settings_save()` di Dinghy, attraverso `libs/storage/hi_score_fm1.h`:

- il record si carica all'accensione (`hi_score_load()`, 0 se non c'è niente o se i dati sono di un altro firmware);
- si salva **dopo** la partita, quando compare il game over: cancellare un settore di flash ferma gli interrupt per circa 50 ms, e durante il gioco si vedrebbe uno scatto;
- `STORE_MAGIC` ("INVD"), definito in `main.c` prima di `platform.c`, distingue i nostri dati da quelli di altri firmware.

## Il ciclo principale

`main.c` è il guscio imperativo attorno alla logica pura:

```c
for (;;) {
    platform_poll();                                   /* USB, tasti, watchdog */
    ... quanti fotogrammi sono dovuti (frame_clock, al massimo 4 di recupero)
    for (; done != due; done++) {
        const Controls controls = input_controls(&CONTROLS, notes, previous_notes, presses, COIN_BUTTON);
        game = game_step(&game, &controls);
    }
    ... salva il record se è cambiato e la partita è finita
    render_game(&back, &game);
    fb_present(&front, &back);
    light_keys(notes);
    screen_push(&front, &map, palette);
}
```

Quando il ciclo recupera più fotogrammi (dopo un salvataggio, per esempio), una pressione conta solo nel primo: un colpo resta un colpo.

## Provarlo

```sh
make host GAME=space-invaders
```

Lo script `games/space-invaders/scripts/demo.txt` preme PLAY, si muove, spara e passa al bianco e nero: le immagini qui sopra vengono da lì.
