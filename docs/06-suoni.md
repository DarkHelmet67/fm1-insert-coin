# 06 · I suoni

La versione web non usa file audio: ogni effetto è descritto da pochi numeri (un tono che scivola da una frequenza all'altra, o un soffio di rumore filtrato, con un volume che tiene e poi cala) e lo suona il sintetizzatore del browser, Web Audio. I numeri vengono dalle misure sulle registrazioni del cabinato originale. Sul synth il browser non c'è, e nemmeno i numeri con la virgola: serve un piccolo sintetizzatore a interi che, con gli stessi numeri, suoni allo stesso modo.

## Gli stessi numeri

`games/space-invaders/src/sounds.c` è `sound-bank.ts` riga per riga, in unità intere: hertz, millisecondi, volume in millesimi.

```c
/* web: { kind: 'tone', wave: 'triangle', sweep: 'linear', from: 2660, to: 2420,
 *        duration: 0.33, hold: 0.16, fade: 'linear', volume: 0.15 } */
static const Sound ALIEN_HIT[] = {
    {SOUND_TONE, WAVE_TRIANGLE, CURVE_LINEAR, CURVE_LINEAR, 2660, 2420, 0, 0, 330, 160, 150},
};
```

Un campo lasciato a zero prende il valore predefinito del web (curva esponenziale, nessun filtro, nessun ritardo, nessuna tenuta), così le righe restano corte come in TypeScript.

Anche *quando* suonare è tradotto da `sounds.ts`: `sounds_for(&prima, &dopo)` confronta lo stato prima e dopo un fotogramma e restituisce i suoni come bit. Un colpo sparato in più è il laser, un effetto "alien" appena nato è l'invasore colpito, una nuova passata della formazione è la nota successiva della marcia. La logica del gioco non sa niente dell'audio, come nel web.

## Un sintetizzatore a interi: `libs/audio`

| File | Cosa fa | Web Audio |
| --- | --- | --- |
| `sound.h` | I suoni come dati: `Sound`, `SoundEffect` (più strati) | `sound.ts` |
| `voice.h/.c` | Un suono che suona: oscillatore o rumore, filtro, inviluppo | `OscillatorNode`, `BiquadFilterNode`, `GainNode` |
| `mixer.h/.c` | 16 voci insieme | la destinazione che somma i nodi |
| `pow2.h/.c` | Potenze e logaritmi di due in virgola fissa | `exponentialRampToValueAtTime` |
| `audio_fm1.h` | La coda fra il ciclo principale e l'interrupt audio | `AudioContext` |

### Oscillatori e rumore

La fase dell'oscillatore è un numero a 32 bit che fa un giro completo quando trabocca: a ogni campione si somma un passo proporzionale alla frequenza (2³² / 44.100 ≈ 97.392 per hertz). Dalla fase escono le forme d'onda: l'onda quadra è il bit più alto, la dente di sega i bit alti così come sono, il triangolo la dente di sega piegata a metà. Il rumore è un generatore xorshift: tre scorrimenti e tre XOR per campione.

### Curve "pari all'orecchio" senza virgola mobile

Web Audio fa scivolare le frequenze e calare i volumi in modo esponenziale: l'orecchio sente uguali gli intervalli in ottave, non in hertz. Con gli interi il trucco è lavorare in ottave: `log2_q16()` trasforma la partenza e l'arrivo in ottave (un'ottava = 65.536), la curva diventa una retta, e `scale_pow2()` riporta il risultato in hertz o in volume. Bastano due tabelle da 65 valori con l'interpolazione fra un valore e l'altro.

Altezza e volume si ricalcolano ogni 32 campioni (0,7 ms), non a ogni campione: l'orecchio non sente la differenza, il processore sì. E i prodotti fra un numero di blocchi (al massimo qualche migliaio) e un valore in Q15 o Q16 restano dentro i 32 bit, senza divisioni a 64 bit, che il chip non ha.

### Il filtro

Il passa-basso del web è un biquad a 12 dB per ottava. Qui sono due filtri a un polo in fila (y += (x − y) · a), che cadono con la stessa pendenza; per perdere 3 dB alla frequenza di taglio, come il biquad, i poli stanno a una volta e mezza il taglio.

## Il confronto con la versione web

"Suona uguale" si può misurare. `make sounds INSERT_COIN=../insert-coin`:

1. `tools/sounds/render_sounds.c` suona ogni effetto con il sintetizzatore C e lo scrive in `build/sounds/c/`;
2. `tools/sounds/render-web-sounds.mjs` prende i numeri da `sound-bank.ts` e li fa suonare al **vero** sintetizzatore del web (`packages/audio/src/synth.ts`) dentro Chromium, con un `OfflineAudioContext` che calcola l'audio senza altoparlanti, e scrive `build/sounds/web/`;
3. `tools/sounds/compare.py` confronta le coppie a finestre di 10 ms: la forma dell'inviluppo (correlazione, 1 = identica), il volume complessivo e l'altezza (dagli attraversamenti dello zero, in finestre di 50 ms).

Il risultato:

| Suono | Inviluppo (correlazione) | Volume C / web | Altezza C / web |
| --- | --- | --- | --- |
| alienHit | 1.00 | +0.0 dB | +0 cent |
| cannonHit | 0.95 | -0.5 dB | rumore |
| coin | 1.00 | +0.1 dB | +0 cent |
| extraLife | 1.00 | -0.0 dB | +0 cent |
| march0 | 0.88 | +1.0 dB | +0 cent |
| march1 | 0.86 | +1.2 dB | +0 cent |
| march2 | 0.94 | +1.1 dB | +0 cent |
| march3 | 0.94 | +0.9 dB | +0 cent |
| newRecord | 1.00 | +0.0 dB | +0 cent |
| shot | 1.00 | +0.0 dB | +0 cent |
| ufo | 1.00 | +0.0 dB | +14 cent |
| ufoHit | 1.00 | +0.1 dB | +0 cent |

Toni e sweep coincidono. Le note della marcia hanno una correlazione un po' più bassa solo perché a 52-69 Hz un periodo dura più di una finestra di 10 ms e il valore efficace oscilla con la fase; l'altezza è esatta. Il rumore è diverso per natura (il web usa `Math.random`) ma ha lo stesso inviluppo e lo stesso volume entro mezzo decibel.

Due correzioni sono uscite proprio da questa misura: l'onda quadra del web è limitata in banda e normalizzata sui picchi, quindi la sua parte piatta è circa 1,5 dB più bassa (`SQUARE_LEVEL`); e i poli del filtro a una volta e mezza il taglio hanno riportato il rumore dell'esplosione al livello giusto.

## Sul synth

`main.c` collega le due metà:

```c
const GameState previous = game;
game = game_step(&previous, &controls);
play_sounds(sounds_for(&previous, &game));     /* audio_play() per ogni bit */
...
static void app_audio(int32_t *out, uint32_t n)
{
    audio_render(out, n);                      /* nell'interrupt, ogni 5,8 ms */
}
```

Fra il ciclo principale e l'interrupt c'è una coda di 16 posti: solo il ciclo scrive l'indice di scrittura, solo l'interrupt quello di lettura, quindi nessuno dei due deve fermare l'altro. L'uscita è mono, uguale sui due canali, come l'altoparlante del cabinato; la manopola MASTER del synth regola il volume.

Il record si salva solo quando nessun suono è in corso (`audio_is_playing()`): cancellare la flash ferma anche l'interrupt audio per circa 50 ms, e si sentirebbe.

In modalità host il suono finisce in `build/host/space-invaders.wav`: lo script di prova fa sentire la moneta, la marcia, i colpi e gli invasori colpiti.
