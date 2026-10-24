# Formato delle tracce di riferimento

Una traccia registra una partita della versione web, fotogramma per fotogramma. Il porting in C rigioca gli stessi comandi e deve scrivere le stesse righe. Questo file è la specifica condivisa dal registratore (`record-space-invaders.mjs`) e dal confronto in C.

## Il file

```
# Space Invaders reference trace "aiming", recorded from insert-coin ddf125b3...
# by tools/traces/record-space-invaders.mjs; format in tools/traces/FORMAT.md
# initial e8c203ae
1 --- A e8c203ae
...
1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18
```

Le righe che iniziano con `#` sono commenti; `# initial` dà l'hash dello stato prima del primo fotogramma. Ogni altra riga è un fotogramma:

| Campo | Significato |
| --- | --- |
| `1234` | Il numero del fotogramma, da 1 |
| `RF-` | I comandi dati in quel fotogramma: direzione `L`, `R` o `-`; fuoco `F` o `-`; moneta `C` o `-`. Il fuoco e la moneta sono già fronti di pressione, come `Controls` della versione web |
| `P` | La schermata dopo il fotogramma: `A` attract, `P` partita, `G` game over |
| `0b4bf1d3` | FNV-1a a 32 bit, in esadecimale, della stringa di stato qui sotto |
| `s= l= r= n=` | Punteggio, cannoni, round, invasori rimasti: solo per chi legge, il confronto li ignora |

## La stringa di stato

Testo ASCII, numeri interi in decimale. Una schermata è una di queste:

```
A|hi=<record>
P|hi=<record>|rec=<record da battere>|<partita>
O|hi=<record>|left=<fotogrammi di game over rimasti>|<partita>
```

La partita è una serie di campi separati da `;`, in quest'ordine:

| Campo | Contenuto |
| --- | --- |
| `r=` `s=` `l=` | Round, punteggio, cannoni compreso quello in gioco |
| `xl=` | 1 se il cannone extra è già stato dato |
| `sf=` | Colpi sparati nella partita |
| `c=` | Bordo sinistro del cannone |
| `ce=` | Fotogrammi di esplosione del cannone rimasti, o `-` |
| `sh=` | Il colpo del cannone `x:y`, o `-` |
| `fl=` | La formazione: `cursore:direzione:discesa(0/1):nota:attesa:` e poi gli invasori in ordine di marcia, separati da `,`, ognuno `<tipo><riga>.<colonna>@<x>:<y>:<fotogramma>` con tipo `s` squid, `c` crab, `o` octopus |
| `b=` | Le bombe: `turno:indice plunger:indice squiggly:` e poi le bombe attive `<tipo>@<x>:<y>:<passi>`, tipo `r` rolling, `p` plunger, `q` squiggly |
| `sd=` | Gli scudi, separati da `,`: `x:y:` e la bitmap 22 × 16 riga per riga, ogni riga in 6 cifre esadecimali (bit più significativo a sinistra, gli ultimi 2 bit a zero) |
| `u=` | Il disco volante: `conto alla rovescia:` e poi `x:direzione:età` o `-` |
| `e=` | Gli effetti, separati da `,`: `<tipo>@<x>:<y>:<fotogrammi>` con `:<punti>` per il disco volante; tipo `alien`, `shot`, `bomb`, `ufo` |

Lo stato dell'attract (il tempo dell'animazione) e il modo colore non entrano nella stringa: sono presentazione, non logica.

## Le partite registrate

| Traccia | Fotogrammi | Cosa copre |
| --- | --- | --- |
| `idle` | 6000 | Nessuno gioca: le bombe distruggono i tre cannoni, game over, ritorno all'attract |
| `restless` | 6000 | Avanti e indietro, un colpo ogni 20 fotogrammi |
| `aiming` | 20000 | Un giocatore che mira e schiva: secondo round, cannone extra, disco volante |

La moneta arriva sempre al fotogramma 60.
