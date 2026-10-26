# 11 · Breakout: a paddle on any encoder

Breakout (Atari, 1976) is the second cartridge. The cabinet had a knob for the paddle and the FM-1 has seven encoders, so whichever one is at hand moves it. This guide follows the port: the logic, exact to the web version over 57,000 frames, the controls and the screen.

<p align="center"><img src="images/breakout-game.png" width="240" alt="A game of Breakout on the FM-1, in host mode"></p>

## How to play

| Action | On the synth | In the simulator |
| --- | --- | --- |
| Move the paddle | Turn any encoder: SELECT, PRESETS, ALGORITHM or KNOB 1-4 | Left and right arrows (or A and D), the mouse wheel on an encoder |
| Start a game, serve | PLAY | Enter |
| Colour film or black and white | FX | V |

Each encoder click moves the paddle 4 scan lines (`KNOB_PIXELS` in `config.h`). The clicks of every encoder are added up, then turned into the position the web version's mouse or finger would give: `app_controls()` sets the pointer to the paddle's centre plus the clicks, and the game logic is the web version's, untouched.

## The logic

One C module per TypeScript file, as for Space Invaders ([guide 04](04-logic.md)):

| Module | What it does |
| --- | --- |
| `ball.c`, `speed.c` | The ball keeps directions and a few flags, as the circuit did, not a velocity: its speed is worked out from the flags at every frame |
| `bricks.c`, `brick_hit.c` | The wall as one flag per brick (the original's 256 × 1 RAM chip); the ball against them |
| `paddle.c` | The paddle's left edge and width; it shrinks once the ball reaches the top wall |
| `serve.c` | The ball circles invisibly after SERVE and shows up when it crosses the middle |
| `ticks.c` | A brick's points are added one at a time with a tick each, trailing behind the ball |
| `play.c`, `game.c`, `attract.c` | Waiting, serving, playing, game over; the second wall; the attract mode that plays by itself |
| `hud.c`, `render.c` | Seven-segment scores (`libs/screen/seven_segment`), everything drawn in one ink, then the coloured film strips tint their bands |
| `sounds.c` | The circuit's square-wave beeps, found by comparing the state before and after a frame |

**The ball on exact fifths.** Every speed in the web version is a multiple of 0.2 pixels, so the C port counts fifths of a pixel in integers (`FIFTHS` in `playfield.h`): no floating point, which the chip lacks, and no rounding. The web version adds the speeds in floating point, which drifts: after enough frames a ball that should touch the right wall misses it by 0.00000000000003 pixels. The trace recorder puts the ball back on the nearest fifth after every frame, the only liberty taken with the TypeScript ([FORMAT.md](../tools/traces/FORMAT.md)).

## Proof

Three traces recorded from the web version, replayed by `parity_test.c`:

| Trace | Frames | What it covers |
| --- | --- | --- |
| `idle` | 6,000 | Three balls lost, back to the attract mode |
| `restless` | 6,000 | The paddle left and right in turn |
| `tracker` | 45,000 | A player who follows the ball: breakthrough, speed-ups, the shrunk paddle, the second wall |

Every frame of the C game has the same state as the web version's. Each module also has its tests (`make test`).

## Screen and colours

The 228 × 208 screen fits the display centred, pixel for pixel (`screen_map_centered`). The colours of the film strips come from the MAME artwork, as in the web version; FX switches to the bare monitor.
