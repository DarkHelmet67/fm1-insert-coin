/* SPDX-License-Identifier: GPL-3.0-only */
#include "attacks.h"
#include "objects.h"
#include "test/check.h"

static Attacks attacks;
static Objects objects;
static Slots slots;
static Capture capture;

/** The bomber settings of the tests. */
static const BomberSettings SETTINGS = {{8, 3, 3}, 5, 2, 0};

/** Inputs at frame `frame` with `flying` aliens in flight. */
static AttackInputs inputs(int32_t frame, int32_t flying)
{
    const AttackInputs in = {frame, SETTINGS, flying, 1, 1, 0};
    return in;
}

/** A manager whose countdowns are `boss`, `red`, `yellow`, with the `count` aliens `ids` home. */
static AttackWorld world(int32_t boss, int32_t red, int32_t yellow, const uint8_t *ids, int count)
{
    const AttackWorld w = {&attacks, &objects, &slots, &capture, 0};
    attacks = new_attacks();
    attacks.countdown[0] = (uint8_t)boss;
    attacks.countdown[1] = (uint8_t)red;
    attacks.countdown[2] = (uint8_t)yellow;
    objects = empty_objects();
    for (int i = 0; i < count; i++) {
        GameObject *o = object_ref(&objects, ids[i]);
        o->disposition = DISPOSITION_HOME;
        o->x = 100;
        o->y = 200;
    }
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        slots.list[i] = free_slot();
    capture = no_capture();
    return w;
}

static void it_waits_for_the_16th_frame(void)
{
    static const uint8_t IDS[] = {0x08};
    AttackWorld w = world(5, 1, 5, IDS, 1);
    const AttackInputs in = inputs(15, 0);
    run_attacks(&w, &in);
    CHECK_EQ(attacks.countdown[1], 1);
    CHECK(!w.dived);
}

static void the_first_resting_bee_leaves_when_its_countdown_runs_out(void)
{
    static const uint8_t IDS[] = {0x08, 0x0a};
    AttackWorld w = world(5, 5, 1, IDS, 2);
    const AttackInputs in = inputs(16, 0);
    run_attacks(&w, &in);
    CHECK(w.dived);
    CHECK_EQ(object_at(&objects, 0x08)->disposition, DISPOSITION_FLYING);
    CHECK_EQ(slots.list[0].pointer, PROGRAM_BEE);
    CHECK_EQ(slots.list[0].bomb_bits, 5);
    CHECK_EQ(attacks.countdown[0], 4);
    CHECK_EQ(attacks.countdown[1], 4);
    CHECK_EQ(attacks.countdown[2], 3);
}

static void too_many_aliens_flying_hold_the_launch(void)
{
    static const uint8_t IDS[] = {0x40};
    AttackWorld w = world(5, 1, 5, IDS, 1);
    const AttackInputs in = inputs(16, 2);
    run_attacks(&w, &in);
    CHECK(!w.dived);
    CHECK_EQ(attacks.countdown[0], 4);
    CHECK_EQ(attacks.countdown[1], 1);
    CHECK_EQ(attacks.countdown[2], 5);
}

static void a_boss_leaves_with_two_escorts_worth_1600_points(void)
{
    static const uint8_t IDS[] = {0x30, 0x32, 0x34, 0x36, 0x48, 0x4a, 0x50, 0x52};
    AttackWorld w = world(1, 5, 5, IDS, 8);
    AttackInputs in = inputs(16, 0);
    run_attacks(&w, &in);
    CHECK_EQ(attacks.queue_length, 3);
    const uint8_t boss = attacks.queue[0].object;
    CHECK_EQ(attacks.boss_scores[(boss & 7) >> 1].hundreds, 0x0d);
    CHECK_EQ(attacks.boss_scores[(boss & 7) >> 1].code, 0xba);
    in.frame = 17;
    run_attacks(&w, &in);
    CHECK_EQ(object_at(&objects, boss)->disposition, DISPOSITION_FLYING);
    CHECK_EQ(attacks.queue_length, 2);
}

static void every_second_boss_launch_is_a_capture_with_no_escorts(void)
{
    static const uint8_t IDS[] = {0x30, 0x32, 0x48, 0x4a};
    AttackWorld w = world(1, 5, 5, IDS, 4);
    attacks.boss_toggle = 1;
    const AttackInputs in = inputs(16, 0);
    run_attacks(&w, &in);
    CHECK(capture.busy);
    CHECK_EQ(capture.object, 0x30);
    CHECK_EQ(attacks.queue_length, 1);
    CHECK_EQ(attacks.queue[0].object, 0x30);
    CHECK(!attacks.queue[0].mirrored);
    CHECK_EQ(attacks.queue[0].program, PROGRAM_CAPTURE);
}

static void nothing_happens_while_the_fighter_cannot_fire(void)
{
    static const uint8_t IDS[] = {0x40};
    AttackWorld w = world(5, 1, 5, IDS, 1);
    AttackInputs in = inputs(16, 0);
    in.fire_enabled = 0;
    run_attacks(&w, &in);
    CHECK_EQ(attacks.countdown[1], 1);
    CHECK(!w.dived);
}

int main(void)
{
    RUN(it_waits_for_the_16th_frame);
    RUN(the_first_resting_bee_leaves_when_its_countdown_runs_out);
    RUN(too_many_aliens_flying_hold_the_launch);
    RUN(a_boss_leaves_with_two_escorts_worth_1600_points);
    RUN(every_second_boss_launch_is_a_capture_with_no_escorts);
    RUN(nothing_happens_while_the_fighter_cannot_fire);
    return check_report();
}
