/* SPDX-License-Identifier: GPL-3.0-only */
#include "formation.h"
#include "object_states.h"
#include "objects.h"
#include "test/check.h"

static Objects objects;
static Slots slots;
static int32_t counting, alive;
static Formation formation;
static const ObjectWorld WORLD = {&objects, &slots, &counting, &alive};

/** Nothing but object `id`, returned to be set up. */
static GameObject *only(int32_t id)
{
    objects = empty_objects();
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        slots.list[i] = free_slot();
    counting = alive = 0;
    formation = initial_formation();
    return object_ref(&objects, id);
}

/** The object manager at `frame`. */
static void step(int32_t frame)
{
    const ObjectInputs inputs = {frame, &formation, 1};
    update_object_states(&WORLD, &inputs);
}

static void a_resting_alien_goes_to_its_cell_and_is_counted(void)
{
    GameObject *bee = only(0x08);
    bee->disposition = DISPOSITION_HOME;
    bee->code = 0x18;
    step(1);
    CHECK(object_at(&objects, 0x08)->x > 0);
    CHECK_EQ(counting, 1);
    step(2);
    CHECK_EQ(alive, 1);
}

static void odd_objects_are_handled_on_frame_3_only(void)
{
    only(0x0a)->disposition = DISPOSITION_ENTERING;
    step(1);
    CHECK_EQ(object_at(&objects, 0x0a)->disposition, DISPOSITION_ENTERING);
    step(3);
    CHECK_EQ(object_at(&objects, 0x0a)->disposition, DISPOSITION_LAUNCHED);
}

static void an_alien_home_upside_down_turns_back_upright(void)
{
    GameObject *bee = only(0x08);
    bee->disposition = DISPOSITION_ROTATING;
    bee->code = 0x1a;
    bee->flip_y = 1;
    for (int32_t frame = 1; frame <= 37; frame += 4)
        step(frame);
    CHECK_EQ(object_at(&objects, 0x08)->disposition, DISPOSITION_HOME);
    CHECK_EQ(object_at(&objects, 0x08)->code, 0x1e);
    CHECK(!object_at(&objects, 0x08)->flip_y);
}

static void a_returning_alien_turns_upright_then_rests(void)
{
    GameObject *bee = only(0x08);
    bee->disposition = DISPOSITION_ROTATING;
    bee->code = 0x1d;
    step(1);
    CHECK_EQ(object_at(&objects, 0x08)->code, 0x1e);
    step(5);
    CHECK_EQ(object_at(&objects, 0x08)->disposition, DISPOSITION_HOME);
}

/** An alien exploding from step $43, with `hit` (1, or a score sprite) and the pose `flip`. */
static void exploding(int32_t hit, int flip)
{
    GameObject *bee = only(0x08);
    bee->disposition = DISPOSITION_EXPLODING;
    bee->counter = 0x43;
    bee->hit = (uint8_t)hit;
    bee->x = 100;
    bee->y = 100;
    bee->flip_x = bee->flip_y = (uint8_t)flip;
}

static void an_explosion_plays_and_is_removed(void)
{
    exploding(1, 0);
    step(1);
    CHECK_EQ(object_at(&objects, 0x08)->code, 0x44);
    CHECK(object_at(&objects, 0x08)->big);
    CHECK_EQ(object_at(&objects, 0x08)->x, 92);
    step(5);
    step(9);
    CHECK_EQ(object_at(&objects, 0x08)->disposition, DISPOSITION_INACTIVE);
    CHECK_EQ(object_at(&objects, 0x08)->x, 0);
}

static void the_score_of_a_hit_shows_upright_whatever_the_pose(void)
{
    exploding(0x35, 1);
    step(1);
    CHECK(!object_at(&objects, 0x08)->flip_x && !object_at(&objects, 0x08)->flip_y);
    step(5);
    step(9);
    CHECK_EQ(object_at(&objects, 0x08)->disposition, DISPOSITION_SHOWING_SCORE);
    CHECK_EQ(object_at(&objects, 0x08)->code, 0x35);
    CHECK(!object_at(&objects, 0x08)->flip_x && !object_at(&objects, 0x08)->flip_y);
}

int main(void)
{
    RUN(a_resting_alien_goes_to_its_cell_and_is_counted);
    RUN(odd_objects_are_handled_on_frame_3_only);
    RUN(an_alien_home_upside_down_turns_back_upright);
    RUN(a_returning_alien_turns_upright_then_rests);
    RUN(an_explosion_plays_and_is_removed);
    RUN(the_score_of_a_hit_shows_upright_whatever_the_pose);
    return check_report();
}
