/* SPDX-License-Identifier: GPL-3.0-only */
#include "program.h"

#include "attack_waves.h"
#include "badges.h"
#include "endings.h"
#include "fighter.h"

/** Frames between two badges, each with its "click" (`c_build_token_1`). */
#define BADGE_FRAMES 8

/** Rows of the texts in the middle of the screen. */
#define MESSAGE_ROW 18
#define ABOVE_ROW 16

/** The object a rogue fighter enters as, with the last wave (`l_2681`). */
#define ROGUE_FIGHTER 0x04

/** Sets the phase to `step`. */
static void go_to(PlayingState *state, int32_t step)
{
    const Phase phase = {(uint8_t)step, 0, 0, 0, 0};
    state->phase = phase;
}

/** Frames since the phase started are not kept: the badges use the frame counter. */
static int badge_due(const PlayingState *state)
{
    return (state->frame & (BADGE_FRAMES - 1)) == 0;
}

/** `stg_init_env`: the new stage's aliens, waves, grid and parameters. */
static void set_up_stage(PlayingState *state)
{
    /* A fighter still held by the aliens comes back with the last wave (`l_2681`). */
    int rogue = state->capture.busy && !state->dual;
    StageWaves waves;
    build_attack_waves(&waves, state->stage, state->rank, state->frame, rogue);
    state->params = stage_parameters(state->stage, state->rank);
    state->timers = set_timer(&state->timers, 2, 120);
    state->timers = set_timer(&state->timers, 0, 2);
    state->looks = stage_looks(state->stage);
    if (rogue) {
        const AlienLook look = {0x00, 7, 1};
        state->looks.look[ROGUE_FIGHTER >> 1] = look;
        state->looks.known[ROGUE_FIGHTER >> 1] = 1;
    }
    start_launcher(&state->launcher, &waves);
    state->entry_bomb_bits = waves.entry_bomb_bits;
    state->bomb_interval = waves.bomb_interval;
    state->attacks = new_attacks();
    state->max_bombers = state->params.max_bombers;
    state->flying_hits = 0;
    state->triple_left = 0;
    state->group_left = is_challenge_stage(state->stage) ? 8 : 0;
    state->formation = initial_formation();
    /* The aliens' hit notifications are cleared for the new stage. */
    for (int32_t i = 0; i < ENEMY_OBJECTS; i++)
        state->objects.list[i].hit = 0;
    set_tasks(state, TASK_SWAY, TASK_BREATHING | TASK_ATTACKS | TASK_CLONES);
    state->waves_enabled = 0;
    state->sway_stops = 0;
    state->star_control.target = (uint8_t)star_speed_target(state->stage);
}

void start_splash(PlayingState *state, int32_t then)
{
    int32_t stage = state->stage + 1;
    int challenge = is_challenge_stage(stage);
    const Message title = challenge ? message_of(MESSAGE_CHALLENGING_STAGE) : stage_message(stage);
    state->stage = stage;
    state->badges = 0;
    state->messages = messages_show(&state->messages, &title);
    state->timers = set_timer(&state->timers, 2, 3);
    state->enemies_enabled = 1;
    go_to(state, STEP_SPLASH);
    state->phase.then = (uint8_t)then;
    if (challenge)
        play(state, SOUND_CHALLENGE_START);
}

/** `c_player_respawn`: "READY" unless the stage title is shown; wait for the aliens. */
static void start_respawn(PlayingState *state)
{
    const Message ready = message_of(MESSAGE_READY);
    set_tasks(state, TASK_STICK, 0);
    if (!messages_shown_at(&state->messages, ready.column, MESSAGE_ROW))
        state->messages = messages_show(&state->messages, &ready);
    go_to(state, STEP_RESPAWN);
}

/** `plyr_respawn_rdy`: the fire button works, waves may start, the texts go away. */
static void start_play(PlayingState *state)
{
    set_tasks(state, TASK_FIRE, 0);
    state->waves_enabled = 1;
    state->messages = messages_erase_row(&state->messages, MESSAGE_ROW);
    state->messages = messages_erase_row(&state->messages, ABOVE_ROW);
    go_to(state, STEP_RUNNING);
}

/** One frame of the stage title: a badge every 8 frames, then the pause runs out. */
static void splash(PlayingState *state, int32_t then)
{
    if (state->badges < (int32_t)stage_flag_count(state->stage)) {
        if (!badge_due(state))
            return;
        state->badges++;
        if (!is_challenge_stage(state->stage))
            play(state, SOUND_BADGE);
        return;
    }
    if (state->timers.t[2] != 0)
        return;
    set_up_stage(state);
    switch (then) {
    case THEN_PLAYER_UP:
        show_message(state, MESSAGE_PLAYER_ONE_ABOVE);
        start_respawn(state);
        return;
    case THEN_RESPAWN:
        start_respawn(state);
        return;
    default:
        start_play(state);
        return;
    }
}

/** `c_133A`: when nothing flies, the new fighter appears and the stars start moving. */
static void respawn(PlayingState *state)
{
    if (flying_count(state) != 0)
        return;
    int32_t timer2 = state->timers.t[2] + 0x1e;
    place_fighter(&state->objects);
    state->star_control.ship_on_screen = 1;
    set_tasks(state, TASK_FIGHTER_HITS, 0);
    state->restart = 0;
    state->timers = set_timer(&state->timers, 2, timer2 < 0x78 ? timer2 : 0x78);
    state->timers = set_timer(&state->timers, 3, 3);
    go_to(state, STEP_RESPAWN_DELAY);
}

/** `gctl_stg_restart_hdlr`: a pause of 4 timer ticks. */
static void start_restart(PlayingState *state)
{
    state->timers = set_timer(&state->timers, 3, 4);
    go_to(state, STEP_RESTART);
}

/** `gctl_supv_stage`: the stage ends when no alien is left, or restarts on a lost fighter. */
static void running(PlayingState *state)
{
    if (state->alive == 0 && !state->launcher.running) {
        start_restart(state);
    } else if (state->restart) {
        state->waves_enabled = 0;
        start_restart(state);
    }
}

/** The start of a scripted sequence. */
static void start_ending(PlayingState *state, int32_t script)
{
    go_to(state, STEP_ENDING);
    state->phase.script = (uint8_t)script;
}

/**
 * After the pause: a lost fighter is replaced (or the game is over), a cleared stage is followed
 * by the next one.
 */
static void restart(PlayingState *state)
{
    /* `l_04A4`: a rescued fighter still landing takes the place of the lost one. */
    if ((state->capture.tasks & CAPTURE_RESCUE) != 0) {
        state->restart = 0;
        set_tasks(state, TASK_FIGHTER_HITS, 0);
        if (state->alive > 0)
            go_to(state, STEP_RUNNING);
        return;
    }
    if (state->timers.t[3] != 0)
        return;
    if (state->restart || state->alive > 0) {
        if (state->reserve == 0) {
            start_ending(state, SCRIPT_GAME_OVER);
            return;
        }
        state->reserve--;
        if (state->alive == 0)
            start_splash(state, THEN_RESPAWN);
        else
            start_respawn(state);
        return;
    }
    if (is_challenge_stage(state->stage))
        start_ending(state, state->flying_hits == 40 ? SCRIPT_PERFECT : SCRIPT_CHALLENGE);
    else
        start_splash(state, THEN_READY);
}

/**
 * One frame of a scripted sequence: run the next line, or wait. At the end, the next stage
 * follows the challenge results, the attract screen follows the game.
 */
static void ending(PlayingState *state)
{
    int32_t script = state->phase.script;
    int32_t line = state->phase.line;
    if (line >= script_length(script)) {
        if (script == SCRIPT_GAME_OVER)
            go_to(state, STEP_OVER);
        else
            start_splash(state, THEN_READY);
        return;
    }
    Wait wait = script_wait(script, line);
    if (state->phase.waiting) {
        if (wait_over(state, wait)) {
            state->phase.line = (uint8_t)(line + 1);
            state->phase.waiting = 0;
        }
        return;
    }
    run_script_line(state, script, line);
    start_wait(state, wait);
    go_to(state, STEP_ENDING);
    state->phase.script = (uint8_t)script;
    state->phase.line = (uint8_t)line;
    state->phase.waiting = 1;
}

void start_intro(PlayingState *state)
{
    show_message(state, MESSAGE_PLAYER_ONE);
    state->timers = set_timer(&state->timers, 3, 8);
    go_to(state, STEP_INTRO);
    play(state, SOUND_START_THEME);
}

void run_program(PlayingState *state)
{
    switch (state->phase.step) {
    case STEP_INTRO:
        if (state->timers.t[3] == 0) {
            state->messages = messages_erase_row(&state->messages, MESSAGE_ROW);
            start_splash(state, THEN_PLAYER_UP);
        }
        return;
    case STEP_SPLASH:
        splash(state, state->phase.then);
        return;
    case STEP_RESPAWN:
        respawn(state);
        return;
    case STEP_RESPAWN_DELAY:
        if (state->timers.t[3] == 0)
            start_play(state);
        return;
    case STEP_RUNNING:
        running(state);
        return;
    case STEP_RESTART:
        restart(state);
        return;
    case STEP_ENDING:
        ending(state);
        return;
    default:
        return;
    }
}
