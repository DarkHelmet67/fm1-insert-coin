# 04 · The game logic in C

The longest step of the port: rewrite the web version's Space Invaders logic in C, module by module, until the C game produces **exactly** the same games. The result is measured with the traces of [guide 03](03-traces.md): 32,000 frames over three games, and in every one the C state has the same hash as the TypeScript state.

## From TypeScript to C, file by file

The structure stays the same as insert-coin's, so whoever knows one repository finds their way in the other:

| TypeScript (insert-coin) | C (`games/space-invaders/src/`) | What it does |
| --- | --- | --- |
| `sprites.ts` | `sprites.h/.c` | The drawings, one string per pixel row, identical |
| `aliens.ts` | `aliens.h/.c` | The starting formation, widths, points, round heights |
| `fleet.ts` | `fleet.h/.c` | The march: one invader per frame, descent, reversal |
| `cannon.ts`, `shot.ts` | `cannon.h/.c` | The cannon and its shot |
| `bombs.ts` | `bombs.h/.c` | The three bombs taking turns, the column tables, reloading |
| `ufo.ts` | `ufo.h/.c` | The flying saucer and the 23rd-shot trick |
| `shields.ts` | `shields.h/.c` | The crumbling shields |
| `effects.ts` | `effects.h/.c` | Short-lived explosions and splashes |
| `collisions.ts` | `collisions.h/.c` | The collision rules, one function each |
| `playing.ts` | `playing.h/.c` | One frame of play: movement, then the rules in order |
| `game.ts` | `game.h/.c` | Attract screen, game, game over, high score |
| `controls.ts` | `controls.h` | Direction, fire, coin: what the player does in a frame |

Drawing, sounds and debug mode are left out: drawing and sounds come in steps 5 and 6, debug mode is not needed on the synth.

## The same style, in C

The web version is functional: every function receives an immutable state and returns a new one. In C the style stays the same, with one simple idea: **the struct is passed by constant pointer and returned by value**.

```c
/* TypeScript: export const stepUfo = (state: UfoState, aliensLeft, shotsFired): UfoState => ... */
Ufo ufo_step(const Ufo *ufo, uint32_t aliens_left, uint32_t shots_fired)
{
    Ufo next = *ufo;              /* a local copy */
    if (ufo->countdown > 0) {
        next.countdown--;         /* only the copy changes */
        return next;
    }
    ...
}
```

TypeScript's immutable lists (`readonly Alien[]`) become fixed-size arrays with a counter: 55 invaders, 3 bombs, 4 shields, at most 24 effects. No `malloc`: there is no allocator on the chip and none is needed.

The collision rules apply as in the TypeScript, where they are an array of functions passed to `reduce`:

```c
static const Rule COLLISION_RULES[] = {
    shot_hits_alien,  shot_hits_ufo,     shot_hits_bomb,   shot_hits_shield,
    bombs_hit_cannon, bombs_hit_shields, bombs_hit_ground, aliens_crush_shields,
    award_extra_life, next_round_if_cleared,
};
for (uint32_t i = 0; i < RULE_COUNT; i++)
    current = COLLISION_RULES[i](&current);
```

### What copying costs

A game's state (`PlayingState`) takes about 1 KB: every rule makes a copy, some twenty copies per frame, that is about 20 KB of `memcpy` every 16.7 ms. For a 320 MHz processor that is nothing. The firmware's stack is 24 KB: measured with `-fstack-usage`, the deepest path (`game_step` → `playing_step` → a rule) uses about 9.

## Pixel collisions: `libs/sprite`

The drawings stay as readable as in the TypeScript, one string per row:

```c
SPRITE_DEFINE(SQUID_0, 8,
    "...XX...",
    "..XXXX..",
    ...);
```

A string is awkward for collisions, so `bitmap_from_sprite()` converts it into a `Bitmap`: one 32-bit word per row, bit 31 is the leftmost pixel. On this work `placed_overlap()` (first the common rectangle, then the pixels one by one, like `bitmapsOverlap` in `@arcade/collision`) and `bitmap_erase()` (the holes in the shields). The library is shared: Galaga and Breakout will use it too.

## The traps of translation

The code is almost a line-by-line translation, but some differences between JavaScript and C are silent traps: the game works, it just is not the same game any more. The traces find all of them.

- **Division with negative numbers.** `Math.floor(-5 / 2)` is −3, in C `-5 / 2` is −2. A narrow invader's explosion is centred with a negative number, which is why `effects.c` has `floor_div()`.
- **Non-integer numbers.** The TypeScript moves the cannon and the shot with speeds in pixels per second multiplied by 1/60. Checked in Node: `60 × (1/60)` and `240 × (1/60)` give exactly 1 and 4, so the positions stay integers and integers are enough in C.
- **The "before" state and the "after" state.** In `advanceWorld` the bombs and the flying saucer see the formation *before* this frame's step, because the TypeScript object literal reads `state` for every field. The C code passes the old fleet to `bombs_step`, not the one just moved.
- **Removal by identity.** TypeScript's `removeBombs` filters by reference; the C code removes by index, walking from the end when it removes more than one so as not to shift the indexes still to be used.
- **Shields hit by several bombs.** In the same frame all bombs are checked against the shields as they were before the rule, then the damage is applied one after the other: exactly as `reduce` does in the TypeScript.
- **"No value".** `undefined` becomes a field: `active` for the shot, `flying` for the flying saucer, `cannon_explosion` at 0 while the cannon is alive (in the TypeScript it never drops below 1 before becoming `undefined`), `NO_POINTS` for effects with no score.

## The parity test

`parity_test.c` reads the three traces, replays each frame's controls and compares the hash of the C state with the recorded one. `trace_state.c` writes the C state as the same canonical string as [`tools/traces/FORMAT.md`](../tools/traces/FORMAT.md).

```
$ make test
== build/host/games/space-invaders/src/parity_test
- idle_matches_the_web_version
- restless_matches_the_web_version
- aiming_matches_the_web_version
- the_text_never_overflows
4 checks, 0 failed
```

At the first difference the test stops and prints the frame and the C state string. Comparing it with the TypeScript one finds the wrong field:

```bash
node tools/traces/record-space-invaders.mjs ../insert-coin --dump aiming 574
```

For example, with a 7-pixel descent instead of 8 the test says so at once:

```
idle: frame 611 differs, trace 381ec416, C 0b827da5
P|hi=0|rec=0|r=1;s=0;l=3;xl=0;sf=0;c=16;ce=-;sh=-;fl=1:-1:1:10:4:o5.1@44:135:1,...
```

The first invader is at y = 135 instead of 136. The test is a safety net for everything to come: if a change to the C code alters even a single pixel of one shield in just one of the 32,000 frames, `make test` fails.

## The unit tests

Next to parity, every module has its tests, translated from the web version's (`aliens_test.c`, `fleet_test.c`, `bombs_test.c`, `ufo_test.c`, `cannon_test.c`, `shields_test.c`, `effects_test.c`, `collisions_test.c`, `playing_test.c`, `game_test.c`). Parity says *whether* the game is the same, the unit tests say *where* it broke and explain how each rule works: `it_moves_the_last_invader_3_pixels_right_but_only_2_left` reads like the documentation of a quirk of the original.

## The prompt to do it with an AI

> Translate the logic modules of insert-coin's `games/space-invaders/src/` into C99, one per file, with the same name. State passed as `const T *` and returned by value, fixed arrays instead of lists, integers only. Then write `trace_state.c`, which produces the string of `tools/traces/FORMAT.md`, and a test that replays the traces and compares the hashes frame by frame. If a frame differs, compare the C string with the `--dump` one and fix the module, not the trace.

With the "translate, then check against the traces" approach, full parity came at the first attempt: a faithful translation of the TypeScript, with the traps above in mind, was enough.
