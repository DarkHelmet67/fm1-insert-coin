/* SPDX-License-Identifier: GPL-3.0-only */
#include "flight_slot.h"

#include "z80_math.h"

FlightSlot free_slot(void)
{
    FlightSlot slot = new_slot(0);
    slot.used = 0;
    return slot;
}

FlightSlot new_slot(int32_t object)
{
    FlightSlot slot;
    slot.used = 1;
    slot.y = 0;
    slot.x = 0;
    slot.angle = 0;
    slot.target_y = 0;
    slot.target_x = 0;
    slot.pointer = 0;
    slot.speed_odd = 0;
    slot.speed_even = 0;
    slot.turn = 0;
    slot.timer = 1;
    slot.bomb_timer = 0;
    slot.bomb_bits = 0;
    slot.object = (uint8_t)object;
    slot.offset_x = 0;
    slot.offset_y = 0;
    slot.dive_check = 0;
    slot.homing = 0;
    slot.mirrored = 0;
    return slot;
}

int32_t coarse(int32_t value)
{
    return (value >> 8) & 0xff;
}

Pose pose_for(int32_t angle)
{
    const int32_t quadrant = (angle >> 8) & 3;
    const int32_t low = (quadrant & 1) ? ~angle & 0xff : angle & 0xff;
    Pose pose;
    /* dividing by 42 is done as (a/2 + a/4) / 32: no division on a Z80 if it can be avoided */
    pose.tile = low + 21 > 0xff ? 6 : (((low >> 1) + (low >> 2)) >> 5) & 7;
    pose.flip_x = ((quadrant ^ (quadrant >> 1)) & 1) == 0;
    pose.flip_y = (quadrant & 2) != 0;
    return pose;
}

/** The secondary axis moves in these directions in the 8 octants: an octagonal sine. */
static const int32_t SECONDARY_SIGN[8] = {1, 1, -1, 1, -1, -1, 1, -1};

FlightPoint step_position(int32_t x, int32_t y, int32_t angle, int32_t speed)
{
    FlightPoint point = {x, y};
    if (speed == 0)
        return point;
    const int32_t octant = (angle >> 7) & 7;
    const int y_is_primary = ((octant ^ (octant >> 1)) & 1) == 1;
    const int32_t primary = (octant >= 3 && octant <= 6 ? -speed : speed) * 128;
    const int32_t fraction = (angle & 0x80) ? (angle & 0x7f) ^ 0x7f : angle & 0x7f;
    const int32_t secondary = SECONDARY_SIGN[octant] * speed * fraction;
    const int32_t dx = y_is_primary ? secondary : primary;
    const int32_t dy = y_is_primary ? primary : secondary;
    point.x = (x + dx) & 0xffff;
    point.y = (y + dy) & 0xffff;
    return point;
}

SpritePoint sprite_position(const FlightSlot *slot)
{
    SpritePoint point;
    point.x = (slot->x >> 7) & 0xff;
    point.y = (0x161 - ((slot->y >> 7) & 0x1ff)) & 0x1ff;
    if (slot->homing) {
        point.x = (point.x + slot->offset_x) & 0xff;
        point.y = (point.y + signed8(slot->offset_y)) & 0x1ff;
    }
    return point;
}
