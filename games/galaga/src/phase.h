/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Where the main program is (the `Phase` of program.ts), and what it does next. The original is
 * a loop that waits on timers while the tasks run; the port keeps the point of the sequence it
 * has reached and checks each frame whether the wait is over.
 */
#ifndef GALAGA_PHASE_H
#define GALAGA_PHASE_H

#include <stdint.h>

/** The steps of the main program. */
typedef enum {
    STEP_INTRO,         /**< "PLAYER 1" while the start theme plays */
    STEP_SPLASH,        /**< "STAGE n" and the badges; then what `then` says */
    STEP_RESPAWN,       /**< waiting for the aliens to return home before a new fighter */
    STEP_RESPAWN_DELAY, /**< the new fighter is on screen; play resumes after a pause */
    STEP_RUNNING,       /**< the stage is being played */
    STEP_RESTART,       /**< the stage is over or the fighter was lost: a pause */
    STEP_ENDING,        /**< a scripted sequence: challenge results or the end of the game */
    STEP_OVER,          /**< the game is over: back to the attract screen */
} Step;

/** What follows the stage title. */
typedef enum { THEN_PLAYER_UP, THEN_RESPAWN, THEN_READY } SplashThen;

/** The scripted sequences (endings.h). */
typedef enum { SCRIPT_CHALLENGE, SCRIPT_PERFECT, SCRIPT_GAME_OVER } ScriptName;

/** Where the main program is. */
typedef struct {
    uint8_t step;    /**< a Step */
    uint8_t then;    /**< splash: a SplashThen */
    uint8_t script;  /**< ending: a ScriptName */
    uint8_t line;    /**< ending: the line of the script */
    uint8_t waiting; /**< ending: the line has run, its wait is going on */
} Phase;

#endif
