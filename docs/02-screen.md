# 02 The screen

The second step builds the screen layer shared by the games: every game draws at its original resolution, and a library decides where it falls on the FM-1's 240 × 240 display and which rows to send. The result shows in a test card, with a block that the game keys move at 60 frames per second.

<p align="center"><img src="images/space-invaders-step2.png" width="240" alt="The Space Invaders test card: the borders, five coloured bars, the block, the green line and the reserve cannons"></p>

## The problem

| Game | Original resolution | On the 240 × 240 display |
| --- | --- | --- |
| Breakout | 228 × 208 | Fits whole, centred |
| Space Invaders | 224 × 256 | 16 rows too many |
| Galaga | 224 × 288 | 48 rows too many |

Columns are never a problem: every game is at most 228 pixels wide. Rows are. Scaling 256 rows to 240 would mean losing one every sixteen, and 8-row sprites would come out deformed. The second constraint is speed: the display receives its pixels on an SPI bus, and a whole image takes about 31 ms, more than the 16.7 ms of a frame.

## Three pieces

**The framebuffer** (`libs/screen/framebuffer.c`) is the game's screen: one byte per pixel, the index of one of 16 colours, and one bit per row saying whether the row changed. `fb_set()` marks the row only if the pixel really changes, so redrawing an object that stands still costs nothing. For Galaga that is 224 × 288 = 64 KB: on the FM-1 the framebuffer lives in the 336 KB memory pool.

**The map** (`libs/screen/screen_map.c`) says, for each of the display's 240 rows, which game row it shows, or none. A single mechanism covers every case:

- `screen_map_centered(228, 208)`: Breakout centred, 16 black rows above and below;
- `screen_map_rows(224, rows, n)`: only some ranges of rows, stacked. Space Invaders shows rows 8-239 (the top 8 are empty in the original) and 243-250 (the reserve cannons), skipping the three empty rows below the green line: 232 + 8 = 240, pixel for pixel;
- for Galaga its own step will decide: the same mechanism can crop, skip rows or repeat them.

**Sending** (`libs/screen/screen_push.c`) finds the display rows to send: `screen_next_band()` returns the next range of consecutive changed rows, at most as long as the buffer, and `screen_render_row()` colours a row with the palette.

## The link to the hardware

The three libraries are pure C, with their tests. What talks to the display is `libs/screen/screen_fm1.h`, included by `main.c` after `platform.c`. `screen_push_band()` sends one band per call: it finds the next band from where the last one ended, colours it into a buffer and hands it to `lcd_blit()`; when no band is left, it forgets the changes and starts again from the top.

```c
const ScreenBand band = screen_next_band(fb, map, screen_from, SCREEN_BAND_ROWS);
if (band.count == 0) {
    fb_clear_dirty(fb);
    screen_from = 0;
    return 0;
}
uint16_t *px = screen_band_px[screen_which];
for (uint32_t r = 0; r < band.count; r++)
    screen_render_row(fb, map, band.y + r, palette, &px[r * map->width]);
lcd_blit(map->dest_x, band.y, map->width, band.count, px);
screen_which ^= 1u;
screen_from = band.y + band.count;
return 1;
```

One band at a time, because a screen that changes everywhere takes longer than a frame to send (Galaga's stars move on every row). The main loop sends a band, checks whether a frame is due, sends the next: the game keeps its 60 frames a second, and its sound with it, while the display catches up. A new frame is drawn only once the previous one is all on the display, so it never shows half old and half new.

`lcd_blit()` returns at once and leaves the work to the DMA, which keeps reading the buffer. That is why there are two buffers: while one travels, the other fills. The next call to `lcd_blit()` waits for the previous transfer to end, so a buffer is never rewritten while in flight. The colours go through `screen_palette()`, which puts them in the display's byte order.

## Time

The original game runs at 60 frames per second; the FM-1 counts milliseconds. `frame_clock_frames(ms)` (`libs/clock/`) says how many frames they are worth: exactly 60 every 1000 ms, with no drift and without the 64-bit division the chip cannot do. The main loop runs the frames that are due, at most four in a row if it fell behind (for example during a flash save), draws the latest once the display has the previous one, then sends a band.

```c
const uint32_t due = frame_clock_frames(fm1_ms);
if (due - done > MAX_CATCH_UP)
    done = due - MAX_CATCH_UP;
if (done != due) {
    for (; done != due; done++)
        console_step(&console, &panel);
    drawn = 0;
}
if (!drawn && !sending) {
    draw_frame(shown_palette);
    drawn = 1;
}
sending = screen_push_band(&front, &map, shown_palette);
```

## On macOS

Host mode also compiles on the Mac, with one fix in the `Makefile`: the Mach-O format wants section names in the form `segment,section`, while Dinghy declares its large buffers in `.pool`. On the computer the pool means nothing, and the `Makefile` renames it on macOS only.
