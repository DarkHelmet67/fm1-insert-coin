/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The parity test: replays the reference traces recorded from the web version
 * (games/galaga/traces, docs/03-traces.md) and checks, frame after frame, that the C game
 * reaches exactly the same state. Run from the repository root (make test does).
 * On the first difference it prints the frame and the C state string: compare it with
 *   node tools/traces/record-galaga.mjs INSERT_COIN_DIR --dump TRACE FRAME
 */
#include <stdio.h>

#include "game.h"
#include "test/check.h"
#include "trace/trace_line.h"
#include "trace_state.h"

/** Where the traces are, from the repository root. */
#define TRACE_DIR "games/galaga/traces/"

/** The state string of a frame. */
static Text text;

/** The game being replayed: a few kilobytes, kept off the stack. */
static GameState state;

/** The hash of the state, as the trace writes it. */
static uint32_t state_hash(void)
{
    trace_state(&state, &text);
    return text_fnv1a(&text);
}

/** Prints the first difference between the trace and the C game. */
static void report(const char *name, uint32_t frame, uint32_t expected)
{
    trace_state(&state, &text);
    printf("%s: frame %u differs, trace %08x, C %08x\n%s\n", name, (unsigned)frame,
           (unsigned)expected, (unsigned)text_fnv1a(&text), text_cstr(&text));
}

/** Replays one trace; returns the frames checked, 0 when a frame differs or the file is missing. */
static uint32_t replay(const char *name)
{
    char path[128];
    char line[256];
    snprintf(path, sizeof path, TRACE_DIR "%s.trace", name);
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("%s: cannot open %s\n", name, path);
        return 0;
    }
    create_game(&state, 0);
    uint32_t frames = 0;
    unsigned initial = 0;
    while (fgets(line, sizeof line, file)) {
        TraceLine expected;
        if (sscanf(line, "# initial %x", &initial) == 1 && initial != state_hash()) {
            report(name, 0, initial);
            frames = 0;
            break;
        }
        if (line[0] == '#' || !trace_parse_line(line, &expected))
            continue;
        const Controls controls = {(int8_t)expected.direction, (uint8_t)expected.fire,
                                   (uint8_t)expected.coin};
        update_game(&state, &controls);
        if (state_hash() != expected.hash || trace_screen_letter(&state) != expected.screen) {
            report(name, expected.frame, expected.hash);
            frames = 0;
            break;
        }
        frames++;
    }
    fclose(file);
    return frames;
}

static void idle_matches_the_web_version(void)
{
    CHECK_EQ(replay("idle"), 9000u);
}

static void restless_matches_the_web_version(void)
{
    CHECK_EQ(replay("restless"), 9000u);
}

static void rescuer_matches_the_web_version(void)
{
    CHECK_EQ(replay("rescuer"), 19000u);
}

static void survivor_matches_the_web_version(void)
{
    CHECK_EQ(replay("survivor"), 21000u);
}

static void the_text_never_overflows(void)
{
    CHECK(!text.overflow);
}

int main(void)
{
    RUN(idle_matches_the_web_version);
    RUN(restless_matches_the_web_version);
    RUN(rescuer_matches_the_web_version);
    RUN(survivor_matches_the_web_version);
    RUN(the_text_never_overflows);
    return check_report();
}
