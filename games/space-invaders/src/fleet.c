/* SPDX-License-Identifier: GPL-3.0-only */
#include "fleet.h"

Fleet fleet_create(uint32_t round)
{
    Fleet fleet;
    for (uint32_t i = 0; i < FORMATION_SIZE; i++)
        fleet.aliens[i] = formation_alien(i, round);
    fleet.count = FORMATION_SIZE;
    fleet.cursor = 0;
    fleet.direction = 1;
    fleet.dropping = 0;
    fleet.beat = 0;
    fleet.beat_cooldown = 0;
    return fleet;
}

/** Horizontal step of the moving invader, including the last-invader quirk. */
static int32_t step_x(const Fleet *fleet)
{
    if (fleet->direction < 0)
        return -STEP_X;
    return fleet->count == 1 ? LAST_ALIEN_STEP_RIGHT : STEP_X;
}

/** The invader after its step: sideways or down, switching animation frame. */
static Alien move_alien(const Fleet *fleet, const Alien *alien)
{
    Alien moved = *alien;
    if (fleet->dropping)
        moved.y = (int16_t)(moved.y + STEP_DOWN);
    else
        moved.x = (int16_t)(moved.x + step_x(fleet));
    moved.frame = alien->frame ? 0 : 1;
    return moved;
}

int fleet_touches_edge(const Fleet *fleet)
{
    for (uint32_t i = 0; i < fleet->count; i++) {
        const Alien *alien = &fleet->aliens[i];
        if (fleet->direction > 0 ? alien->x + alien_width(alien) >= FLEET_RIGHT_LIMIT
                                 : alien->x <= FLEET_LEFT_LIMIT)
            return 1;
    }
    return 0;
}

/**
 * Starts a new pass once every invader has moved: after a drop the formation marches sideways
 * again; after a sideways pass that reached the edge it drops and turns around.
 */
static Fleet start_next_pass(const Fleet *fleet)
{
    Fleet next = *fleet;
    next.cursor = 0;
    if (fleet->beat_cooldown == 0) {
        next.beat = fleet->beat + 1u;
        next.beat_cooldown = MIN_BEAT_FRAMES;
    }
    if (fleet->dropping) {
        next.dropping = 0;
    } else if (fleet_touches_edge(fleet)) {
        next.dropping = 1;
        next.direction = (int8_t)(fleet->direction > 0 ? -1 : 1);
    }
    return next;
}

Fleet fleet_step(const Fleet *fleet)
{
    Fleet next = *fleet;
    if (next.beat_cooldown > 0)
        next.beat_cooldown--;
    if (fleet->cursor >= fleet->count)
        return next;
    next.aliens[fleet->cursor] = move_alien(fleet, &fleet->aliens[fleet->cursor]);
    next.cursor++;
    return next.cursor >= next.count ? start_next_pass(&next) : next;
}

Fleet fleet_remove(const Fleet *fleet, uint32_t index)
{
    if (index >= fleet->count)
        return *fleet;
    Fleet next = *fleet;
    for (uint32_t i = index; i + 1u < fleet->count; i++)
        next.aliens[i] = fleet->aliens[i + 1u];
    next.count--;
    if (index < fleet->cursor)
        next.cursor--;
    /* if the destroyed invader was the last one due to move in this pass, the pass is over */
    return next.cursor >= next.count && next.count > 0 ? start_next_pass(&next) : next;
}

int32_t fleet_bottom(const Fleet *fleet)
{
    int32_t bottom = INT32_MIN;
    for (uint32_t i = 0; i < fleet->count; i++)
        if (fleet->aliens[i].y + ALIEN_HEIGHT > bottom)
            bottom = fleet->aliens[i].y + ALIEN_HEIGHT;
    return bottom;
}
