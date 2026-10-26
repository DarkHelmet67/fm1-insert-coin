/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The five attack waves that bring the aliens into the formation at the start of each stage
 * (attack-waves.ts, `c_25A2`). Each wave is a list of aliens with a control byte that picks the
 * flight program, the start point and the timing; from stage 4 on, "transients" join the waves:
 * aliens that fly in and leave without taking a place in the formation.
 */
#ifndef GALAGA_ATTACK_WAVES_H
#define GALAGA_ATTACK_WAVES_H

#include <stdint.h>

#include "cartridge/private.h"

/** What a token of the wave table is. */
typedef enum {
    TOKEN_START, /**< a wave starts */
    TOKEN_END,   /**< no more waves */
    TOKEN_ENTRY, /**< one alien: its control byte and its object */
} TokenKind;

/** A token of the table read by the wave launcher. */
typedef struct {
    uint8_t kind; /**< a TokenKind */
    uint8_t control;
    uint8_t object;
} WaveToken;

/** The most tokens: 5 waves of a start mark and 16 aliens, the rogue fighter, the end mark. */
#define MAX_TOKENS (5 * 17 + 2)

/** What `c_25A2` produces: the token list and the two header bytes of the stage. */
typedef struct {
    WaveToken tokens[MAX_TOKENS];
    uint8_t count;
    uint8_t bomb_interval;   /**< header byte 0 (`b_92E2[0]`): frames between bomb chances */
    uint8_t entry_bomb_bits; /**< header byte 1 (`b_92E2[1]`): bomb bits while entering */
} StageWaves;

/**
 * `c_25A2`: the attack waves of `stage` at difficulty `rank`, with the random bytes of `seed`
 * (random.h). The original mixes the Z80 refresh register with the frame counter, something no
 * emulation of the game logic can reproduce: any source of random bytes gives the same kind of
 * waves. When a Boss holds a captured fighter, that fighter flies in with the last wave
 * (`rogue_fighter`).
 */
GAME_PRIVATE void build_attack_waves(StageWaves *waves, int32_t stage, int32_t rank, int32_t seed,
                                     int rogue_fighter);

#endif
