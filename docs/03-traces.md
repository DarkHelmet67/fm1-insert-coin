# 03 The reference traces

How do you prove that the C port is the same game as the web version? Watching it is not enough: an invader firing one frame late, or a wrong flying saucer score, escape the eye. The third step records three complete games from the TypeScript version, frame by frame, in a format the C code can replay and compare.

## The idea

The web version's logic is a pure function: `updateGame(state, controls)` returns the state of the next frame. Nothing is random: bombs follow fixed tables, the flying saucer a counter. So the same controls always produce the same game. It is enough to record the controls and the state of every frame: the C code will receive the same controls and must produce the same states.

A frame's whole state is large (55 invaders, four 22 × 16 pixel shields), so the trace does not store it in full. It writes it as a text string with precise rules and stores a 32-bit fingerprint of it, the FNV-1a hash:

```
1234 RF- P 0b4bf1d3 s=650 l=2 r=1 n=18
```

Frame 1234, controls "right and fire", playing screen, fingerprint `0b4bf1d3`; the numbers at the end are for human readers only. The rules of the string are in [tools/traces/FORMAT.md](../tools/traces/FORMAT.md), the specification shared by the recorder and the C code.

## The recorder

`tools/traces/record-space-invaders.mjs` is a Node script that does not contain the game: it loads it from a checkout of insert-coin through insert-coin's own Vite, which compiles the TypeScript and sets `import.meta.env` as in the browser. The TypeScript stays where it is; only the traces come in here.

```sh
git clone https://github.com/DarkHelmet67/insert-coin ../insert-coin
(cd ../insert-coin && pnpm install)
make traces INSERT_COIN=../insert-coin
```

Each trace names, in its header, the insert-coin commit it comes from. If the web version changes, the traces are recorded again.

Three games, all with the coin at frame 60:

| Trace | Frames | What it covers |
| --- | --- | --- |
| `idle` | 6000 | Nobody plays: bombs destroy the three cannons, game over, back to the attract screen |
| `restless` | 6000 | Back and forth every 90 frames, one shot every 20 |
| `aiming` | 20000 | An automatic player that aims and dodges: it reaches the second round and passes 1500 points (extra cannon) |

The automatic player decides by looking at the state, but the trace stores its choices as controls: the C code replays them without needing the player. To tune it I tried five combinations of "how close" a bomb must be before it runs; the best (a bomb below row 120 and within 18 pixels) reaches the second round.

## For debugging

When the C code gets a frame wrong, the fingerprint will not say what. The recorder prints the whole string of any frame:

```sh
node tools/traces/record-space-invaders.mjs ../insert-coin --dump aiming 1234
```

and the C code prints its own: the first differing character points to the wrong field.

## The C side

`libs/trace/` holds what the comparison needs, in pure, tested C:

- `text.c`: a fixed-size text buffer to build the state string without the C library, and `text_fnv1a()`;
- `trace_line.c`: reading a trace line.

The first test crosses the two languages: the fingerprint of `A|hi=0`, the attract screen before any game, computed in C, must be `e8c203ae`, the same one the TypeScript recorder writes at the top of every trace.

The real comparison came with the logic, in step 4: a whole game in C, frame by frame, against the three traces.
