/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The state of a capture (capture-state.ts): the Boss that dives to catch the fighter, its
 * tractor beam, the captured fighter and its rescue. The original keeps it in a handful of bytes
 * (`ds5_928A_captr_status`, `_b_captr_flag`, `_b_bmbr_boss_cobj` ...) shared by five tasks of
 * the main processor; the port keeps the same values, named after their use.
 */
#ifndef GALAGA_CAPTURE_STATE_H
#define GALAGA_CAPTURE_STATE_H

#include <stdint.h>

#include "cartridge/private.h"

/** The five tasks of a capture (`ds_cpu0_task_actv`), as bits of Capture.tasks. */
enum {
    CAPTURE_APPROACH = 1 << 0, /**< `f_21CB`: the capturing Boss dives and turns upright */
    CAPTURE_BEAM = 1 << 1,     /**< `f_2222`: the beam opens, holds, closes */
    CAPTURE_PULL = 1 << 2,     /**< `f_20F2`: the fighter spins up to the Boss */
    CAPTURE_CARRY = 1 << 3,    /**< `f_19B2`: the Boss carries the captured fighter home */
    CAPTURE_RESCUE = 1 << 4,   /**< `f_2000`: a rescued fighter spins down and joins */
};

/** Number of character rows of the beam. */
#define BEAM_ROWS 10

/** Everything about a capture. */
typedef struct {
    /**
     * A capture is going on, or a fighter is held: no other Boss is chosen to capture
     * (`_b_bmbr_boss_cflag`). It stays set while a double fighter flies.
     */
    uint8_t busy;
    uint8_t object; /**< the capturing Boss, then the rescued fighter; 1 when none */
    uint8_t slot;   /**< the flight slot of the capturing Boss (`_b_cboss_slot`) */
    uint8_t beam_x; /**< sprite x the beam is centered on (`captr_status[0]`) */
    /**
     * The beam's progress (`captr_status[1]`): the low nibble counts its rows, bit 6 marks the
     * closing, bit 7 a Boss shot while beaming; during a rescue it counts the landing phases.
     */
    uint8_t step;
    uint8_t countdown;  /**< frames to the next step of the beam (`captr_status[2]`) */
    uint8_t pulling;    /**< the fighter is still being pulled up (`captr_status[3]`) */
    uint8_t show_text;  /**< "FIGHTER CAPTURED" must be shown (`captr_status[4]`) */
    uint8_t beam_speed; /**< frames between beam steps: the stage parameter, 10 while pulling */
    uint8_t connected;  /**< the fighter has reached the Boss (`b_9200[$0D]`) */
    uint16_t beam_rows; /**< which of the 10 rows of the beam are drawn: bit 0 is row 1 */
    uint8_t tasks;      /**< CAPTURE_* bits */
} Capture;

/** No capture going on. */
GAME_PRIVATE Capture no_capture(void);

/** Draws or erases row `row` (1-10) of the beam. */
GAME_PRIVATE void set_beam_row(Capture *capture, int32_t row, int drawn);

/** Whether row `row` (1-10) of the beam is drawn. */
GAME_PRIVATE int beam_row(const Capture *capture, int32_t row);

/** Turns the tasks `on` on and the tasks `off` off. */
GAME_PRIVATE void set_capture_tasks(Capture *capture, int32_t on, int32_t off);

#endif
