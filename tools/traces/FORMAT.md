# Format of the reference traces

A trace records a game of the web version, frame by frame. The C port replays the same controls and must write the same lines. This file is the specification shared by the recorders (`record-space-invaders.mjs`, `record-breakout.mjs`) and the C comparisons. The file and the frame lines are the same for every game; the state string is each game's own.

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

## Breakout

The frame line is the same, with `F` meaning SERVE and the coin always `-`; the screen is `A` (attract) or `P` (a game), and the numbers for human readers are the score, the ball number and the phase. The state string is one line of fields separated by `|`:

| Field | Content |
| --- | --- |
| `A` or `P` | Attract mode or a game |
| `pl=` | The play: `r` ready, `s:<frames left>:<frame SERVE was pressed>` serving, `o` game over, or `i:<x>:<y>:<dir x>:<dir y>:<hits>:<fast>:<outer>:<can hit a brick>` a ball in play, its position in **fifths of a pixel** |
| `w=` | The wall: 28 hex digits, four bricks a digit, row by row from the top left |
| `s=` `b=` | Score, ball number |
| `sh=` `rf=` | 1 if the paddle is shrunk, 1 if the second wall has been given |
| `p=` | The paddle `x:width` |
| `clk=` | The frame counter |
| `hi=` `rec=` | The record, and the record to beat in this game |
| `t=` | The brick ticks: `owed:played:wait` |

The colour mode is not in the string: it is presentation.

**The ball on exact fifths.** The ball's speeds are multiples of 0.2 pixels, so the C port counts fifths of a pixel in integers, exactly. The web version adds them in floating point, and the error grows: after enough frames a ball that should be at 218.8 is at 218.79999999999998, one more step of 1.2 takes it to 219.99999999999997, and the ball, which should touch the right wall (`x + 4 >= 224`), misses it by a hair and flies on for one more frame. That is not a rule of the game, only a rounding accident. The recorder therefore puts the ball back on the nearest fifth after every frame, so the traces record the game the TypeScript means to play, and the C port matches them exactly. It is the only liberty the recorder takes.

| Trace | Frames | What it covers |
| --- | --- | --- |
| `idle` | 6000 | Nobody plays: three balls served and lost, back to the attract mode, which plays by itself |
| `restless` | 6000 | The paddle left and right in turn every 70 frames |
| `tracker` | 45000 | A player who follows the ball with the keys, aiming at five parts of the paddle in turn: it breaks through, speeds up, shrinks the paddle and gets the second wall (frame 44421) |

The game starts with SERVE at frame 60; then SERVE comes whenever a ball is waiting, on the frames that are multiples of 50.
