/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_commands.h"

#include "flight_memory.h"
#include "z80_math.h"

/** The first object of the "transients", the special enemies a bonus bee splits into. */
#define FIRST_TRANSIENT 0x38
#define TRANSIENT_COUNT 4

/** Sets the disposition of object `id`, if it is one. */
static void set_disposition(FlightWorld *world, int32_t id, int32_t disposition)
{
    GameObject *object = object_ref(world->objects, id);
    if (object)
        object->disposition = (uint8_t)disposition;
}

/** A value kept between `low` and `high`. */
static int32_t clamp(int32_t value, int32_t low, int32_t high)
{
    return value < low ? low : value > high ? high : value;
}

Next stop_flying(FlightWorld *world, int32_t index, const FlightSlot *slot)
{
    GameObject *object = object_ref(world->objects, slot->object);
    if (object) {
        object->disposition = DISPOSITION_INACTIVE;
        object->x = 0;
    }
    world->slots->list[index] = free_slot();
    return NEXT_STOP;
}

/**
 * Commands $FE and $F3 pick the length of the next turn from a table of 8 bytes, according to
 * where the fighter is: the aliens "aim" by turning longer or shorter.
 */
static void aimed_turn(FlightSlot *slot, int32_t index)
{
    slot->timer = (uint8_t)rom_byte(slot->pointer + index);
    slot->pointer = (uint16_t)(slot->pointer + 9);
}

/** `case_0B16` ($FE): aim by the fighter's distance from the screen side the alien comes from. */
static void aim_from_side(FlightSlot *slot, const FlightInputs *inputs)
{
    int32_t fighter_x = inputs->fighter_x == 0 ? 0x80 : inputs->fighter_x;
    int32_t distance = ((slot->mirrored ? fighter_x : (0xf2 - fighter_x) & 0xff) + 0x0e) & 0xff;
    aimed_turn(slot, z80_divide(distance << 8, 0x1e) >> 8);
}

/** `case_0A01` ($F3): aim by the horizontal distance between the alien and the fighter. */
static void aim_at_fighter(FlightSlot *slot, const FlightInputs *inputs)
{
    int32_t fighter_x = clamp(inputs->fighter_x, 0x1e, 0xd1);
    int32_t half = signed8((((fighter_x >> 1) - coarse(slot->x)) & 0x1ff) >> 1);
    int32_t offset = (slot->mirrored ? -half : half) + 0x18;
    aimed_turn(slot, clamp(offset, 0, 0x2f) / 6 + 1);
}

/**
 * `case_0AA0` ($FB): head home. The alien's position is moved into the frame of the grid's
 * origin, the offset of its cell is frozen and added back when drawing, and the direction is set
 * straight towards the origin of its cell.
 */
static Next head_home(FlightWorld *world, FlightSlot *slot, const FlightInputs *inputs)
{
    HomeCell cell = home_cell(slot->object);
    int32_t offset_x = formation_offset(inputs->formation, cell.column);
    int32_t offset_y = formation_offset(inputs->formation, cell.row);
    int32_t y = (slot->y + signed8(offset_y) * 128) & 0xffff;
    int32_t x = (slot->x - signed8(offset_x) * 128) & 0xffff;
    int32_t target_y = cell_origin(cell.row);
    int32_t target_x = cell_origin(cell.column) >> 1;
    set_disposition(world, slot->object, DISPOSITION_FLYING);
    slot->x = (uint16_t)x;
    slot->y = (uint16_t)y;
    slot->offset_x = (uint8_t)offset_x;
    slot->offset_y = (uint8_t)offset_y;
    slot->angle = (uint16_t)angle_to(target_y, target_x, coarse(y), coarse(x));
    slot->target_y = (uint8_t)target_y;
    slot->target_x = (uint8_t)target_x;
    slot->homing = 1;
    slot->pointer = (uint16_t)(slot->pointer + 1);
    return NEXT_READ;
}

/** `case_0A53` ($F4): the Boss that will capture the fighter dives towards it. */
static Next capture_dive(FlightWorld *world, int32_t index, FlightSlot *slot,
                         const FlightInputs *inputs)
{
    int32_t aligned = (((inputs->fighter_x + 3) & 0xf8) + 1) & 0xff;
    int32_t target_x = clamp(aligned, 0x29, 0xc9);
    flight_emit(world, EVENT_CAPTURE_DIVE, index, target_x);
    slot->angle = (uint16_t)angle_to(0x48, target_x >> 1, coarse(slot->y), coarse(slot->x));
    slot->pointer = (uint16_t)(slot->pointer + 1);
    return NEXT_READ;
}

/** The first inactive transient, or -1. */
static int32_t free_transient(const FlightWorld *world)
{
    for (int32_t i = 0; i < TRANSIENT_COUNT; i++) {
        int32_t id = FIRST_TRANSIENT + i * 2;
        if (object_at(world->objects, id)->disposition == DISPOSITION_INACTIVE)
            return id;
    }
    return -1;
}

/** The last free flight slot, or -1: the original searches from the end. */
static int32_t last_free_slot(const FlightWorld *world)
{
    for (int32_t i = SLOT_COUNT - 1; i >= 0; i--)
        if (!world->slots->list[i].used)
            return i;
    return -1;
}

/** `case_097B` ($F2): a bonus bee splits off a special enemy that flies its own program. */
static Next split_off(FlightWorld *world, FlightSlot *slot)
{
    int32_t transient = free_transient(world);
    int32_t free = last_free_slot(world);
    if (transient >= 0 && free >= 0) {
        const GameObject *parent = object_at(world->objects, slot->object);
        GameObject *clone_object = object_ref(world->objects, transient);
        FlightSlot clone = *slot;
        clone.target_y = 0;
        clone.target_x = 0;
        clone.offset_x = 0;
        clone.offset_y = 0;
        clone.pointer = (uint16_t)rom_word(slot->pointer + 1);
        clone.speed_odd = 1;
        clone.speed_even = 2;
        clone.timer = 1;
        clone.object = (uint8_t)transient;
        world->slots->list[free] = clone;
        clone_object->code = parent->code;
        clone_object->color = parent->color;
        clone_object->disposition = DISPOSITION_FLYING;
        clone_object->counter = (uint8_t)free;
    }
    slot->pointer = (uint16_t)(slot->pointer + 3);
    return NEXT_READ;
}

/** Continue reading at `pointer`. */
static Next read_at(FlightSlot *slot, int32_t pointer)
{
    slot->pointer = (uint16_t)pointer;
    return NEXT_READ;
}

/** Stop for this frame, resuming at `pointer`. */
static Next wait_at(FlightSlot *slot, int32_t pointer)
{
    slot->pointer = (uint16_t)pointer;
    return NEXT_WAIT;
}

/** $F9: move to the column of the alien's home cell, before a dive from the formation. */
static Next go_to_column(FlightWorld *world, FlightSlot *slot, const FlightInputs *inputs)
{
    int32_t column = home_cell(slot->object).column;
    int32_t x = (formation_coord(inputs->formation, column) >> 1) << 8;
    slot->x = (uint16_t)(x | (slot->x & 0xff));
    if (inputs->continuous_bombing)
        flight_emit(world, EVENT_DIVE_SOUND, 0, 0);
    return wait_at(slot, slot->pointer + 1);
}

/** $F6: a new direction from the program, with the bomb timer reloaded. */
static Next set_direction(FlightSlot *slot, const FlightInputs *inputs)
{
    int32_t raw = rom_byte(slot->pointer + 1);
    int32_t angle = slot->mirrored ? -(raw + 0x80) & 0xff : raw;
    slot->angle = (uint16_t)((angle << 2) & 0x3ff);
    slot->bomb_timer = 0x1e;
    slot->bomb_bits = (uint8_t)inputs->bomb_enable_bits;
    return wait_at(slot, slot->pointer + 2);
}

/** $F1: move just above the alien's home row. */
static Next go_above_row(FlightSlot *slot)
{
    int32_t row = home_cell(slot->object).row;
    int32_t y = ((cell_origin(row) + 0x20) & 0xff) << 8;
    slot->y = (uint16_t)(y | (slot->y & 0xff));
    return wait_at(slot, slot->pointer + 1);
}

Next run_command(int32_t command, FlightWorld *world, int32_t index, FlightSlot *slot,
                 const FlightInputs *inputs)
{
    int32_t p = slot->pointer;
    switch (command) {
    case 0x00:
        return stop_flying(world, index, slot);
    case 0x01:
        aim_from_side(slot, inputs);
        return NEXT_FLY;
    case 0x02:
        return read_at(slot, rom_word(p + 1));
    case 0x03:
        slot->target_y = (uint8_t)rom_byte(p + 1);
        slot->target_x = 0;
        slot->dive_check = 1;
        slot->pointer = (uint16_t)(p + 2);
        return NEXT_FLY;
    case 0x04:
        return head_home(world, slot, inputs);
    case 0x05:
        return inputs->continuous_bombing && !inputs->rescue_running
                   ? read_at(slot, p + 3)
                   : read_at(slot, rom_word(p + 1));
    case 0x06:
        return go_to_column(world, slot, inputs);
    case 0x07:
        slot->y = (uint16_t)((0x9c << 8) | (slot->y & 0xff));
        return wait_at(slot, p + 1);
    case 0x08:
        return (slot->object & 0x38) == 0x38 ? read_at(slot, rom_word(p + 1))
                                             : read_at(slot, p + 3);
    case 0x09:
        return set_direction(slot, inputs);
    case 0x0a:
        set_disposition(world, slot->object, DISPOSITION_LAUNCHED);
        return read_at(slot, p + 1);
    case 0x0b:
        return capture_dive(world, index, slot, inputs);
    case 0x0c:
        aim_at_fighter(slot, inputs);
        return NEXT_FLY;
    case 0x0d:
        return split_off(world, slot);
    case 0x0e:
        return go_above_row(slot);
    case 0x0f:
        return inputs->harder_entries ? wait_at(slot, rom_word(p + 1)) : wait_at(slot, p + 3);
    case 0x10:
        return inputs->harder_attacks ? wait_at(slot, rom_word(p + 1)) : wait_at(slot, p + 3);
    default:
        return stop_flying(world, index, slot);
    }
}
