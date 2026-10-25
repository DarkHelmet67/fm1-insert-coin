/* SPDX-License-Identifier: GPL-3.0-only */
#include "object_states.h"

#include "clock.h"
#include "flight_memory.h"

/** Removes an object from the screen. */
static void remove_object(GameObject *object)
{
    object->disposition = DISPOSITION_INACTIVE;
    object->x = 0;
    object->flip_x = 0;
    object->flip_y = 0;
    object->big = 0;
    object->wide = 0;
}

/** Frees flight slot `index`, if it is one. */
static void free_slot_at(Slots *slots, int32_t index)
{
    if (index >= 0 && index < SLOT_COUNT)
        slots->list[index] = free_slot();
}

/** `l_249B`: a resting alien's sprite is placed at its cell of the grid. */
static void place_at_home(GameObject *object, int32_t id, const Formation *formation)
{
    HomeCell cell = home_cell(id);
    object->x = (uint8_t)formation_coord(formation, cell.column);
    object->y = (uint16_t)formation_coord(formation, cell.row);
}

/** `case_2488`: resting. The wings flap with the slow clock, every 32 frames. Counts. */
static int resting(GameObject *object, int32_t id, const ObjectInputs *inputs)
{
    int32_t wings = (slow_clock(inputs->frame) >> 1) & 1;
    object->code = (uint8_t)((object->code & ~1) | wings);
    if (inputs->enemies_enabled)
        place_at_home(object, id, inputs->formation);
    return 1;
}

/** `case_245F`: back home, the alien turns one drawing at a time until it is upright. Counts. */
static int rotating(GameObject *object, int32_t id, const ObjectInputs *inputs)
{
    int32_t tile = object->code & 7;
    /* Bit 0 of the sprite control, tested and cleared by the original, is the up/down flip. */
    if (object->flip_y) {
        if (tile == 0)
            object->flip_y = 0;
        else
            object->code--;
    } else if (tile == 6) {
        object->disposition = DISPOSITION_HOME;
    } else {
        object->code++;
    }
    place_at_home(object, id, inputs->formation);
    return 1;
}

/**
 * `case_254D`: a flying alien, a rocket or a bomb outside the screen is removed; an alien also
 * frees its flight slot. Returns whether it still counts as an alien in play.
 */
static int leaving(GameObject *object, Slots *slots)
{
    int32_t half = object->y >> 1;
    int outside = object->x >= 0xf4 || half < 0x0b || half >= 0xa5;
    if (!outside)
        return object->disposition != DISPOSITION_SHOT;
    if (object->disposition == DISPOSITION_LAUNCHED)
        free_slot_at(slots, object->counter);
    remove_object(object);
    return 0;
}

int32_t score_palette(int32_t code, int32_t color)
{
    if (code < 0x37)
        return color;
    return code < 0x3a ? 0x0d : 0x0e;
}

/**
 * `l_24E6`: at the end of an explosion the alien disappears, or shows the score of the hit for
 * 19 steps when the hit notification holds a score sprite.
 */
static void end_explosion(GameObject *object)
{
    if (object->hit == 1) {
        remove_object(object);
        return;
    }
    int32_t code = object->hit;
    /* Codes from $3B are drawn double width and stay where they are; the others move to the
     * middle of the explosion. */
    int wide = code >= 0x3b;
    object->disposition = DISPOSITION_SHOWING_SCORE;
    object->counter = 0x13;
    object->code = (uint8_t)code;
    object->color = (uint8_t)score_palette(code, object->color);
    if (!wide)
        object->x = (uint8_t)(object->x + 8);
    object->y = (uint16_t)((object->y + 8) & 0x1ff);
    object->big = 0;
    object->wide = (uint8_t)wide;
    /* l_2529 rewrites the sprite control with the size only: the score is never mirrored. */
    object->flip_x = 0;
    object->flip_y = 0;
}

/** `case_24B2`: an explosion, from step $40 to $45; step $44 is drawn double size. */
static void exploding(GameObject *object)
{
    if (object->counter == 0x45) {
        end_explosion(object);
        return;
    }
    int32_t step = object->counter + 1;
    object->counter = (uint8_t)step;
    if (step == 0x45) {
        object->code = 0x48;
    } else if (step == 0x44) {
        object->code = 0x44;
        object->x = (uint8_t)(object->x - 8);
        object->y = (uint16_t)((object->y - 8) & 0x1ff);
        object->big = 1;
        /* l_24DA writes $0C to the sprite control: double size and no flips, so the explosion
         * and the score after it are drawn upright whatever the alien's pose. */
        object->flip_x = 0;
        object->flip_y = 0;
    } else {
        object->code = (uint8_t)step;
    }
}

/**
 * `case_2535`: the score of a hit stays on screen for a while. The object removed keeps its last
 * counter, 1, as in the web version.
 */
static void showing_score(GameObject *object)
{
    if (object->counter == 1)
        remove_object(object);
    else
        object->counter--;
}

/** `case_243C`: the fighter's explosion: a new drawing every 4 steps, 15 steps in all. */
static void fighter_exploding(GameObject *object)
{
    if (object->counter == 1) {
        remove_object(object);
        return;
    }
    object->counter--;
    if ((object->counter & 3) == 0)
        object->code = (uint8_t)(object->code + 4);
}

/** `case_2422`: a diving alien keeps the offsets of its cell up to date in its flight slot. */
static int diving(const GameObject *object, int32_t id, Slots *slots, const ObjectInputs *inputs)
{
    HomeCell cell = home_cell(id);
    int32_t index = object->counter;
    if (index < SLOT_COUNT && slots->list[index].used) {
        slots->list[index].offset_x = (uint8_t)formation_offset(inputs->formation, cell.column);
        slots->list[index].offset_y = (uint8_t)formation_offset(inputs->formation, cell.row);
    }
    return 1;
}

/** The step of object `id` for its disposition (`d_23FF_jp_tbl`); returns whether it counts. */
static int step(GameObject *object, int32_t id, Slots *slots, const ObjectInputs *inputs)
{
    switch (object->disposition) {
    case DISPOSITION_HOME:
        return resting(object, id, inputs);
    case DISPOSITION_ROTATING:
        return rotating(object, id, inputs);
    case DISPOSITION_LAUNCHED:
    case DISPOSITION_SHOT:
        return leaving(object, slots);
    case DISPOSITION_EXPLODING:
        exploding(object);
        return 0;
    case DISPOSITION_SHOWING_SCORE:
        showing_score(object);
        return 0;
    case DISPOSITION_ENTERING:
        object->disposition = DISPOSITION_LAUNCHED;
        return 1;
    case DISPOSITION_FIGHTER_EXPLODING:
        fighter_exploding(object);
        return 0;
    case DISPOSITION_FLYING:
        return diving(object, id, slots, inputs);
    default:
        return 0;
    }
}

void update_object_states(const ObjectWorld *world, const ObjectInputs *inputs)
{
    int32_t frame = inputs->frame;
    if ((frame & 1) == 0) {
        if ((frame & 2) != 0) {
            *world->alive = *world->counting;
            *world->counting = 0;
        }
        return;
    }
    /* Frame 1 handles objects $00, $04, $08 ...; frame 3 objects $02, $06, $0A ... */
    for (int32_t i = 0; i < OBJECT_COUNT / 2; i++) {
        int32_t id = (frame & 2) + i * 4;
        GameObject *object = object_ref(world->objects, id);
        if (!is_active(object))
            continue;
        if (step(object, id, world->slots, inputs))
            ++*world->counting;
    }
}
