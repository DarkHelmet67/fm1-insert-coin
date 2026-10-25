# Format of the reference traces

A trace records a game of the web version, frame by frame. The C port replays the same controls and must write the same lines. This file is the specification shared by the recorder (`record-space-invaders.mjs`) and the C comparison.

## The file

```
# Space Invaders reference trace "aiming", recorded from insert-coin ddf125b3...
# by tools/traces/record-space-invaders.mjs; format in tools/traces/FORMAT.md
# initial e8c203ae
1 --- A e8c203ae
...
1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18
```

Lines starting with `#` are comments; `# initial` gives the hash of the state before the first frame. Every other line is a frame:

| Field | Meaning |
| --- | --- |
| `1234` | The frame number, from 1 |
| `RF-` | The controls given in that frame: direction `L`, `R` or `-`; fire `F` or `-`; coin `C` or `-`. Fire and coin are already press edges, like `Controls` in the web version |
| `P` | The screen after the frame: `A` attract, `P` playing, `G` game over |
| `0b4bf1d3` | 32-bit FNV-1a, in hexadecimal, of the state string below |
| `s= l= r= n=` | Score, cannons, round, invaders left: for human readers only, the comparison ignores them |

## The state string

ASCII text, integers in decimal. A screen is one of these:

```
A|hi=<high score>
P|hi=<high score>|rec=<record to beat>|<game>
O|hi=<high score>|left=<game over frames left>|<game>
```

The game is a series of fields separated by `;`, in this order:

| Field | Content |
| --- | --- |
| `r=` `s=` `l=` | Round, score, cannons including the one in play |
| `xl=` | 1 if the extra cannon has already been given |
| `sf=` | Shots fired in the game |
| `c=` | The cannon's left edge |
| `ce=` | Cannon explosion frames left, or `-` |
| `sh=` | The cannon's shot `x:y`, or `-` |
| `fl=` | The formation: `cursor:direction:descending(0/1):note:wait:` then the invaders in marching order, separated by `,`, each `<kind><row>.<column>@<x>:<y>:<frame>` with kind `s` squid, `c` crab, `o` octopus |
| `b=` | The bombs: `turn:plunger index:squiggly index:` then the active bombs `<kind>@<x>:<y>:<steps>`, kind `r` rolling, `p` plunger, `q` squiggly |
| `sd=` | The shields, separated by `,`: `x:y:` and the 22 × 16 bitmap row by row, each row as 6 hex digits (most significant bit on the left, the last 2 bits zero) |
| `u=` | The flying saucer: `countdown:` then `x:direction:age` or `-` |
| `e=` | The effects, separated by `,`: `<kind>@<x>:<y>:<frames>` with `:<points>` for the saucer; kind `alien`, `shot`, `bomb`, `ufo` |

The attract state (the animation timer) and the colour mode are not in the string: they are presentation, not logic.

## The recorded games

| Trace | Frames | What it covers |
| --- | --- | --- |
| `idle` | 6000 | Nobody plays: bombs destroy the three cannons, game over, back to the attract screen |
| `restless` | 6000 | Back and forth, one shot every 20 frames |
| `aiming` | 20000 | A player who aims and dodges: second round, extra cannon, flying saucer |

The coin always comes in at frame 60.
