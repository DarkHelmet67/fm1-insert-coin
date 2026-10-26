/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight.h"

#include "flight_bombs.h"
#include "flight_commands.h"
#include "flight_memory.h"
#include "z80_math.h"

/** The first instruction token that is a command. */
#define COMMAND_FIRST 0xef

/** Commands can chain (a jump, then a jump...): a safety limit against a looping program. */
#define MAX_COMMANDS_PER_FRAME 16

/** `l_0BDC`: an ordinary instruction sets two speeds, a rotation and a duration. */
static void load_instruction(FlightSlot *slot)
{
    int32_t p = slot->pointer;
    int32_t speeds = rom_byte(p);
    int32_t turn = rom_byte(p + 1);
    slot->speed_odd = (uint8_t)(speeds & 0x0f);
    slot->speed_even = (uint8_t)(speeds >> 4);
    slot->turn = (uint8_t)(slot->mirrored ? -turn & 0xff : turn);
    slot->timer = (uint8_t)rom_byte(p + 2);
    slot->pointer = (uint16_t)(p + 3);
}

/** Reads instructions from `slot->pointer` until one sets a movement or ends the frame. */
static Next read_program(FlightWorld *world, int32_t index, FlightSlot *slot,
                         const FlightInputs *inputs)
{
    for (int depth = 0;; depth++) {
        int32_t token = rom_byte(slot->pointer);
        if (token < COMMAND_FIRST) {
            load_instruction(slot);
            return NEXT_FLY;
        }
        if (depth >= MAX_COMMANDS_PER_FRAME)
            return stop_flying(world, index, slot);
        Next next = run_command(~token & 0xff, world, index, slot, inputs);
        if (next != NEXT_READ)
            return next;
    }
}

/** Whether two coarse coordinates are at most one unit (2 pixels) apart. */
static int near(int32_t a, int32_t b)
{
    int32_t d = signed8(a - b);
    return d >= -1 && d <= 1;
}

/** `l_0D03`: the sprite of the slot's object follows the slot, drawn for direction `angle`. */
static void place_sprite(FlightWorld *world, const FlightSlot *slot, int32_t angle)
{
    GameObject *object = object_ref(world->objects, slot->object);
    if (!object)
        return;
    Pose pose = pose_for(angle);
    SpritePoint point = sprite_position(slot);
    object->code = (uint8_t)((object->code & 0xf8) | pose.tile);
    object->flip_x = (uint8_t)pose.flip_x;
    object->flip_y = (uint8_t)pose.flip_y;
    object->x = (uint8_t)point.x;
    object->y = (uint16_t)point.y;
}

/**
 * `l_0E08_imhome`: the alien has reached its cell. The slot is freed and the object starts
 * turning upright in the formation; a bonus bee takes back the look of a bee. Only the position
 * of the sprite changes (`l_0D03`): drawing and flips stay those of the last frame of flight.
 */
static void arrive_home(FlightWorld *world, int32_t index, const FlightSlot *slot,
                        const FlightInputs *inputs)
{
    GameObject *object = object_ref(world->objects, slot->object);
    FlightSlot home = *slot;
    home.y = (uint16_t)(slot->target_y << 8);
    home.x = (uint16_t)(slot->target_x << 8);
    SpritePoint point = sprite_position(&home);
    world->slots->list[index] = free_slot();
    if (!object)
        return;
    int bonus_bee = ((object->color + 1) & 7) >= 5;
    object->disposition = DISPOSITION_ROTATING;
    object->x = (uint8_t)point.x;
    object->y = (uint16_t)point.y;
    if (bonus_bee) {
        object->code = (uint8_t)((inputs->bonus_bee_look & 0xf8) + 6);
        object->color = (uint8_t)(inputs->bonus_bee_look & 0x07);
        flight_emit(world, EVENT_BONUS_BEE_HOME, 0, 0);
    }
}

/** `l_0C05`-`l_0DF5`: one frame of flight along the current instruction. */
static void fly(FlightWorld *world, int32_t index, FlightSlot *slot, const FlightInputs *inputs)
{
    if (slot->homing && near(coarse(slot->y), slot->target_y) &&
        near(coarse(slot->x), slot->target_x)) {
        arrive_home(world, index, slot, inputs);
        return;
    }
    int32_t dy = (coarse(slot->y) - slot->target_y) & 0xff;
    if (slot->dive_check && (dy == 0 || dy == 0xff)) {
        slot->timer = 1;
        slot->dive_check = 0;
    }
    int32_t speed = (inputs->frame & 1) == 1 ? slot->speed_odd : slot->speed_even;
    int32_t angle = slot->angle;
    FlightPoint moved = step_position(slot->x, slot->y, angle, speed);
    slot->x = (uint16_t)moved.x;
    slot->y = (uint16_t)moved.y;
    slot->angle = (uint16_t)((angle + signed8(slot->turn)) & 0x3ff);
    /* The sprite is drawn with the direction before this frame's rotation. */
    place_sprite(world, slot, angle);
    try_bombing(world, slot, inputs);
    world->slots->list[index] = *slot;
}

/** Whether an alien with this disposition is under control of a flight program. */
static int is_flying(int32_t disposition)
{
    return disposition == DISPOSITION_LAUNCHED || disposition == DISPOSITION_FLYING ||
           disposition == DISPOSITION_ENTERING;
}

/** One frame of one slot: the "bug motion runner" `f_08D3` for slot `index`. */
static void run_slot(FlightWorld *world, int32_t index, const FlightInputs *inputs)
{
    FlightSlot slot = world->slots->list[index];
    if (!slot.used)
        return;
    if (!is_flying(object_at(world->objects, slot.object)->disposition)) {
        stop_flying(world, index, &slot);
        return;
    }
    slot.timer = (uint8_t)(slot.timer - 1);
    if (slot.timer != 0) {
        fly(world, index, &slot, inputs);
        return;
    }
    switch (read_program(world, index, &slot, inputs)) {
    case NEXT_STOP:
        return;
    case NEXT_WAIT:
        slot.timer = (uint8_t)(slot.timer + 1);
        world->slots->list[index] = slot;
        return;
    default:
        fly(world, index, &slot, inputs);
        return;
    }
}

void run_flights(FlightWorld *world, const FlightInputs *inputs)
{
    for (int32_t index = 0; index < SLOT_COUNT; index++)
        run_slot(world, index, inputs);
}
