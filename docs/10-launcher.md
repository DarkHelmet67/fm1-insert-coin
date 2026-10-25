# 10 · The launcher: one firmware, a menu of games

Space Invaders was a firmware of its own. With Breakout and Galaga coming, three firmwares would have meant reinstalling to switch game, so the synth now runs one firmware, `insert-coin`, that opens on a menu. This guide explains the menu, the interface every game plugs into, and the trick that lets three games with the same file names live in one program.

<p align="center"><img src="images/menu-galaga.png" width="240" alt="The menu with Galaga chosen: its attract screen at half size"></p>

## What the player sees

| Action | On the synth | In the simulator |
| --- | --- | --- |
| Choose a game | Turn SELECT, PRESETS or ALGORITHM (the three encoders left of the screen) | Up and Down arrows, or the mouse wheel on an encoder |
| Enter it | PLAY | Enter |
| Back to the menu | Hold PLAY for 2 seconds | Hold Enter |

The picture in the middle is not a drawing: it is the chosen game's own attract screen, running live and shrunk to half size. Under it are the game's name, its best score and one dot per game. Each game keeps its own best score in the flash; Space Invaders takes over the score saved by its old standalone firmware, so nobody loses a record.

## A game as a cartridge

The launcher knows nothing about Space Invaders, Breakout or Galaga. It knows a **cartridge** (`libs/cartridge/cartridge.h`), a struct of facts and function pointers:

| Field | What it is |
| --- | --- |
| `name`, `record_tag` | The name in the menu; four letters that mark its best score in the flash |
| `state_size`, `width`, `height` | How many bytes of state it needs (at most 16 KB), the size of its screen |
| `screen_map()` | Where its screen lands on the 240 × 240 display ([guide 02](02-screen.md)) |
| `start(state, hi_score)` | Power-on: its attract screen |
| `step(state, panel)` | One 60 Hz frame, with everything the panel did (keys, buttons, encoder clicks) |
| `render(state, fb)` | Draws the whole frame |
| `colors(state)`, `sounds(state)`, `sound_effects` | Its palette now, the effects the last frame started, the effects themselves |
| `voices(state)`, `wsg_prom` | For a game with a Namco sound chip (Galaga): what the chip plays ([guide 12](12-galaga.md)) |
| `hi_score`, `playing`, `keys`, `buttons` | Its best score, whether a game is on, which keys and buttons to light |

The state lives in bytes the launcher lends (`Console.state`, 16 KB); each game's `cartridge.c` casts them back to its own `App`. That file is the only glue: the rest of the game is the same C as before.

`launcher/src/` holds the pure parts (`launcher.c`: menu or game, which one, PLAY held; `menu.c`: the menu's drawing; `records.c`: the best scores), the imperative `console.c` that runs them around the chosen cartridge, and `main.c`, the firmware's only file that talks to the hardware. The browser simulator (`launcher/sim/sim.c`) runs the same console.

## Three games, one name space: the unity build

The three games were written as separate programs and share file and function names: each has a `game.c`, a `render.c`, a `game_step()`. Renaming everything would make the code harder to compare with the web version. Instead each game is compiled as **one file**: `games/<game>/unity.c` defines `GAME_AMALGAMATION` and includes every module of the game. With that macro, `GAME_PRIVATE` (in front of every function and table of a game's headers, see `libs/cartridge/private.h`) becomes `static`, so the game's names stay inside its file. Only `<GAME>_CARTRIDGE` is public.

The tests do not define the macro: there `GAME_PRIVATE` is empty and every module is compiled and linked on its own, as before.

Adding a module to a game means adding it to its `unity.c` too: the firmware's link fails otherwise.

## The menu's colours

The menu draws the game's picture with the game's own palette, so the two share one 128-colour table: the first 112 indexes are the game's (Galaga uses 97, with its 64 star colours), the menu's own colours come after them (`launcher/src/config.h`).

## Tests

- `launcher_test.c`: the three left encoders choose, wrapping round both ways, and do nothing inside a game; PLAY enters; PLAY held for 120 frames goes back, and letting it go starts the count again.
- `menu_test.c`: six-digit scores, the picture at half size and centred, PLAY blinking, no arrows nor dots for a single game, the palette (the game's, then the menu's).
- `records_test.c`: up to 8 records found by their tag, in a fixed table that goes to the flash as it is; rubbish read from the flash is refused.
- `console_test.c`: two pretend cartridges; the game behind the menu runs silently, a record is kept when the game is over or left, the sound chip plays only inside its game.
