/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The parity test of the sound board: replays the controls of the reference traces and checks
 * that the three voices of the WSG chip play what they play in the web version, frame after frame
 * (the .sound files of games/galaga/traces, tools/traces/FORMAT.md). A line of the sound trace
 * hashes 60 frames; on the first difference the test prints the second it falls in. Run from the
 * repository root (make test does).
 */
#include <stdio.h>

#include "audio/wsg.h"
#include "game.h"
#include "sound_board.h"
#include "sound_commands.h"
#include "test/check.h"
#include "trace/text.h"
#include "trace/trace_line.h"

/** Where the traces are, from the repository root. */
#define TRACE_DIR "games/galaga/traces/"

/** Frames hashed together in a line of the sound trace. */
#define SOUND_LINE_FRAMES 60u

static GameState state;
static SoundState sound;
static Text heard;

/** Adds one frame of the sound board to `heard`, as the recorder writes it. */
static void hear_frame(void)
{
    SoundCommands commands;
    WsgFrame frame;
    if (state.screen == SCREEN_PLAYING)
        sound_commands(&state.on.playing, &commands);
    else
        no_sound_commands(&commands);
    sound_frame(&sound, &commands, &frame);
    for (int32_t t = 0; t < WSG_TICKS; t++)
        for (int32_t v = 0; v < WSG_VOICES; v++) {
            text_add_int(&heard, (int32_t)frame.tick[t][v].frequency);
            text_add_char(&heard, ':');
            text_add_int(&heard, frame.tick[t][v].volume);
            text_add_char(&heard, ':');
            text_add_int(&heard, frame.tick[t][v].waveform);
            text_add_char(&heard, ',');
        }
    text_add_char(&heard, commands.explosion ? 'x' : '-');
}

/** The next line of the sound trace that is not a comment: its frame and hash; 0 at the end. */
static int next_sound_line(FILE *file, unsigned *frame, unsigned *hash)
{
    char line[128];
    while (fgets(line, sizeof line, file))
        if (line[0] != '#' && sscanf(line, "%u %x", frame, hash) == 2)
            return 1;
    return 0;
}

/**
 * Replays the controls of trace `name` with the sound board; returns the sound lines matched, 0
 * when one differs or a file is missing.
 */
static uint32_t replay(const char *name)
{
    char path[128];
    char line[256];
    snprintf(path, sizeof path, TRACE_DIR "%s.trace", name);
    FILE *controls_file = fopen(path, "r");
    snprintf(path, sizeof path, TRACE_DIR "%s.sound", name);
    FILE *sound_file = fopen(path, "r");
    uint32_t matched = 0;
    if (!controls_file || !sound_file) {
        printf("%s: a trace is missing\n", name);
    } else {
        create_game(&state, 0);
        sound = new_sound_state();
        text_clear(&heard);
        while (fgets(line, sizeof line, controls_file)) {
            TraceLine frame;
            unsigned at, hash;
            if (line[0] == '#' || !trace_parse_line(line, &frame))
                continue;
            const Controls controls = {(int8_t)frame.direction, (uint8_t)frame.fire,
                                       (uint8_t)frame.coin};
            update_game(&state, &controls);
            hear_frame();
            if (frame.frame % SOUND_LINE_FRAMES != 0)
                continue;
            if (!next_sound_line(sound_file, &at, &hash) || at != frame.frame ||
                hash != text_fnv1a(&heard)) {
                printf("%s: the sound of frames %u-%u differs\n", name,
                       (unsigned)(frame.frame - SOUND_LINE_FRAMES + 1), (unsigned)frame.frame);
                matched = 0;
                break;
            }
            text_clear(&heard);
            matched++;
        }
    }
    if (controls_file)
        fclose(controls_file);
    if (sound_file)
        fclose(sound_file);
    return matched;
}

static void idle_sounds_as_the_web_version(void)
{
    CHECK_EQ(replay("idle"), 9000u / SOUND_LINE_FRAMES);
}

static void restless_sounds_as_the_web_version(void)
{
    CHECK_EQ(replay("restless"), 9000u / SOUND_LINE_FRAMES);
}

static void rescuer_sounds_as_the_web_version(void)
{
    CHECK_EQ(replay("rescuer"), 19000u / SOUND_LINE_FRAMES);
}

static void survivor_sounds_as_the_web_version(void)
{
    CHECK_EQ(replay("survivor"), 21000u / SOUND_LINE_FRAMES);
}

static void the_text_never_overflows(void)
{
    CHECK(!heard.overflow);
}

int main(void)
{
    RUN(idle_sounds_as_the_web_version);
    RUN(restless_sounds_as_the_web_version);
    RUN(rescuer_sounds_as_the_web_version);
    RUN(survivor_sounds_as_the_web_version);
    RUN(the_text_never_overflows);
    return check_report();
}
