/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * Everything about a game in progress (playing.ts). Its parts are passed to the tasks of a frame
 * as pointers into the frame's working copy (see flight.h).
 */
#ifndef GALAGA_PLAYING_H
#define GALAGA_PLAYING_H

#include <stdint.h>

#include "attacks.h"
#include "capture_state.h"
#include "cartridge/private.h"
#include "clock.h"
#include "flight_world.h"
#include "formation.h"
#include "messages.h"
#include "objects.h"
#include "phase.h"
#include "rockets.h"
#include "sound_names.h"
#include "stage_setup.h"
#include "star_control.h"
#include "stars.h"
#include "wave_launcher.h"

/**
 * The tasks of the main processor that the program turns on and off (`ds_cpu0_task_actv`), as
 * bits of PlayingState.tasks.
 */
enum {
    TASK_SWAY = 1 << 0,         /**< `f_2A90`: the grid sways while the waves enter */
    TASK_BREATHING = 1 << 1,    /**< `f_1DE6`: the grid breathes once all aliens are home */
    TASK_ATTACKS = 1 << 2,      /**< `f_1B65`: aliens leave the formation to attack */
    TASK_STICK = 1 << 3,        /**< `f_1F85`: the joystick is read */
    TASK_FIRE = 1 << 4,         /**< `f_1F04`: the fire button is read */
    TASK_CLONES = 1 << 5,       /**< `f_1A80`: the clone attack of the bonus bee */
    TASK_FIGHTER_HITS = 1 << 6, /**< `f_05EE`: aliens and bombs can hit the fighter */
};

/** The bonus for shooting the three aliens of a triple attack. */
typedef struct {
    uint8_t hundreds;
    uint8_t code;
} TripleBonus;

/** Everything about a game in progress. */
typedef struct {
    int32_t frame; /**< frames since the game started (`ds3_92A0_frame_cts[0]`) */
    GameTimers timers;
    Phase phase;   /**< where the main program is */
    uint8_t tasks; /**< TASK_* bits */
    Objects objects;
    Slots slots;
    BombDrift bomb_drift[BOMB_COUNT];
    RocketPaths rocket_paths;
    uint8_t step_toggle; /**< alternating 1 and 2 pixel steps of the fighter */
    uint8_t dual;        /**< two fighters side by side, after a rescue */
    Formation formation;
    StarField stars;
    StarControl star_control;
    int32_t stage; /**< the stage being played (1, 2, ...) */
    int32_t rank;  /**< the difficulty, a Rank */
    StageParameters params;
    Looks looks;
    Launcher launcher;
    uint8_t entry_bomb_bits;
    uint8_t bomb_interval; /**< frames between two bomb chances of a diving alien (`b_92E2[0]`) */
    Attacks attacks;
    int32_t max_bombers; /**< aliens attacking at once: grows once after 30 s (`b_92C0[$09]`) */
    int32_t flying_hits; /**< flying aliens hit in this stage */
    int32_t group_left;  /**< aliens left of the current group of 8 (challenging stages) */
    Capture capture;     /**< the capturing Boss, its beam, the captured or rescued fighter */
    int32_t triple_left; /**< aliens left of a triple attack */
    TripleBonus triple_bonus;
    uint8_t enemies_enabled; /**< resting aliens follow the grid (`ds_9200_glbls[$0B]`) */
    uint8_t waves_enabled;   /**< new waves may start (`_b_atk_wv_enbl`) */
    uint8_t sway_stops;      /**< all waves are home: the sway stops in the middle */
    int32_t alive;           /**< aliens in play, as counted by the object manager */
    int32_t counting;        /**< the count in progress */
    uint8_t restart;         /**< the fighter was lost: the stage restarts (`ds_9200_glbls[$13]`) */
    int32_t reserve;         /**< fighters in reserve (`_b_nships`), shown at the bottom left */
    int32_t badges;          /**< stage badges shown so far: they appear one by one */
    Messages messages;
    int32_t score;
    int32_t next_bonus; /**< the next extra fighter, in tens of thousands (`_b_mcfg_bonus`) */
    int32_t shots;      /**< rockets fired, for the results at the end of the game */
    int32_t hits;       /**< aliens hit, for the results */
    uint32_t sounds;    /**< SOUND_* bits started in this frame */
} PlayingState;

/** A game before its first frame, at `step` of the main program. */
GAME_PRIVATE void empty_playing_state(PlayingState *state, int32_t step);

/** Starts `sound` in this frame. */
GAME_PRIVATE void play(PlayingState *state, uint32_t sound);

/** Aliens in flight (`b_bugs_flying_nbr`): the flight slots in use. */
GAME_PRIVATE int32_t flying_count(const PlayingState *state);

/** Whether all the TASK_* bits `tasks` are on. */
GAME_PRIVATE int task_on(const PlayingState *state, int32_t tasks);

/** Turns the tasks `on` on and the tasks `off` off. */
GAME_PRIVATE void set_tasks(PlayingState *state, int32_t on, int32_t off);

#endif
