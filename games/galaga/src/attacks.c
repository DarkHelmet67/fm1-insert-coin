/* SPDX-License-Identifier: GPL-3.0-only */
#include "attacks.h"

#include "rom.h"
#include "table.h"

/** The Boss of each ordinal 1-4 of the escort search (`c_1C8D`): 4 is the leftmost. */
static const uint8_t BOSS_BY_ORDINAL[4] = {0x32, 0x36, 0x34, 0x30};

/** The four Bosses and the fighters they may hold, in the order they are searched. */
#define FIRST_BOSS 0x30
#define FIRST_CAPTURED 0x00

Attacks new_attacks(void)
{
    Attacks attacks = {{0x16, 2, 2},
                       {{0, 0, 0}},
                       0,
                       0,
                       {{1, 0xb5}, {1, 0xb5}, {1, 0xb5}, {1, 0xb5}},
                       1,
                       0,
                       0x1b,
                       4};
    return attacks;
}

/** Whether object `id` rests in the formation. */
static int at_home(const Objects *objects, int32_t id)
{
    return object_at(objects, id)->disposition == DISPOSITION_HOME;
}

/** Aliens on the right half of the formation fly mirrored programs: bit 1 of their number. */
static int is_right_side(int32_t id)
{
    return (id & 2) != 0;
}

/** The first of the four objects from `first` resting at home, or -1. */
static int32_t first_at_home(const Objects *objects, int32_t first)
{
    for (int32_t i = 0; i < 4; i++)
        if (at_home(objects, first + i * 2))
            return first + i * 2;
    return -1;
}

/** The bits a launched alien may bomb with: none while the aliens are not "enabled". */
static int32_t launch_bomb_bits(const AttackInputs *inputs)
{
    return inputs->enemies_enabled ? inputs->settings.bomb_bits : 0;
}

int32_t first_free_slot(const Slots *slots)
{
    for (int32_t i = 0; i < SLOT_COUNT; i++)
        if (!slots->list[i].used)
            return i;
    return -1;
}

void launch_diver(AttackWorld *world, const QueuedLaunch *launch, int32_t bomb_bits)
{
    int32_t free = first_free_slot(world->slots);
    if (free < 0)
        return;
    GameObject *sprite = object_ref(world->objects, launch->object);
    FlightSlot slot = new_slot(launch->object);
    slot.pointer = launch->program;
    slot.angle = 0x100;
    slot.y = (uint16_t)(((0x161 - sprite->y) & 0x1ff) << 7);
    slot.x = (uint16_t)(sprite->x << 7);
    slot.bomb_timer = 0x1e;
    slot.bomb_bits = (uint8_t)bomb_bits;
    slot.mirrored = launch->mirrored;
    world->slots->list[free] = slot;
    sprite->disposition = DISPOSITION_FLYING;
    sprite->counter = (uint8_t)free;
    world->dived = 1;
}

/** `case_bmbr_yellow`, `case_bmbr_red`: the first resting bee or butterfly but the bonus bee. */
static void launch_first(AttackWorld *world, int32_t first, int32_t count, int32_t program,
                         const AttackInputs *inputs)
{
    for (int32_t i = 0; i < count; i++) {
        int32_t id = first + i * 2;
        if (at_home(world->objects, id) && id != world->attacks->bonus_bee) {
            QueuedLaunch launch = {(uint8_t)id, (uint8_t)is_right_side(id), (uint16_t)program};
            launch_diver(world, &launch, launch_bomb_bits(inputs));
            return;
        }
    }
}

/** Index of a Boss in the score table: $30, $32, $34, $36 are 0-3. */
static int32_t boss_index(int32_t boss)
{
    return (boss & 7) >> 1;
}

/** Puts `object` at the end of the launch queue. */
static void enqueue(Attacks *attacks, int32_t object, int mirrored, int32_t program)
{
    QueuedLaunch *launch = &attacks->queue[attacks->queue_length++];
    launch->object = (uint8_t)object;
    launch->mirrored = (uint8_t)mirrored;
    launch->program = (uint16_t)program;
}

/**
 * `j_1CAE`: a Boss leaves with `count` escorts (0, 1 or 2 butterflies). Its score when shot
 * diving is 400 alone, 800 with one escort, 1600 with two; everyone waits in the queue.
 */
static void queue_boss(AttackWorld *world, int32_t boss, const int32_t *escorts, int32_t count,
                       int32_t program)
{
    Attacks *attacks = world->attacks;
    int mirrored = is_right_side(boss);
    int32_t score_row = 2 - count;
    BossScore *score = &attacks->boss_scores[boss_index(boss)];
    score->hundreds = (uint8_t)table_at(BOSS_SCORES, BOSS_SCORES_SIZE, score_row * 2, 1);
    score->code = (uint8_t)table_at(BOSS_SCORES, BOSS_SCORES_SIZE, score_row * 2 + 1, 0xb5);
    attacks->queue_length = 0;
    enqueue(attacks, boss, mirrored, program);
    for (int32_t i = 0; i < count; i++)
        enqueue(attacks, escorts[i], mirrored, program);
    /* The captured fighter of this Boss, if it rests above it, follows it (`l_1CE3`). */
    if (at_home(world->objects, boss & 7))
        enqueue(attacks, boss & 7, mirrored, program);
}

/**
 * The escorts free under the Boss of ordinal `n` (1-4): three butterflies of the escort row
 * (`d_1D2C_wingmen`). Writes them to `out` and returns how many.
 */
static int32_t free_escorts(const AttackWorld *world, int32_t n, int32_t out[3])
{
    int32_t count = 0;
    for (int32_t bit = 0; bit < 3; bit++) {
        /* The original reads the escort row reversed: index k is ESCORTS[5 - k]. */
        int32_t id = ESCORTS[5 - (4 - n + bit)];
        if (id != world->attacks->bonus_bee && at_home(world->objects, id))
            out[count++] = id;
    }
    return count;
}

/** The first Boss (ordinals 4 to 1) at home with `wanted` free escorts leaves; 0 if none. */
static int pick_boss(AttackWorld *world, int32_t wanted, int32_t program)
{
    for (int32_t n = 4; n >= 1; n--) {
        int32_t escorts[3];
        int32_t boss = BOSS_BY_ORDINAL[n - 1];
        if (free_escorts(world, n, escorts) >= wanted && at_home(world->objects, boss)) {
            queue_boss(world, boss, escorts, wanted, program);
            return 1;
        }
    }
    return 0;
}

/**
 * `l_1C30`: a Boss with escorts. Six butterflies can escort; each Boss looks at the three below
 * it. First a Boss with two free escorts is sought, then one with one, then any Boss alone; with
 * no Boss left, a captured fighter dives by itself.
 */
static void launch_boss(AttackWorld *world, const AttackInputs *inputs)
{
    int32_t program = inputs->enemies_enabled ? PROGRAM_BOSS : PROGRAM_BOSS_TRAINING;
    if (pick_boss(world, 2, program) || pick_boss(world, 1, program))
        return;
    int32_t lone = first_at_home(world->objects, FIRST_BOSS);
    if (lone >= 0) {
        queue_boss(world, lone, 0, 0, program);
        return;
    }
    int32_t rogue = first_at_home(world->objects, FIRST_CAPTURED);
    if (rogue >= 0) {
        QueuedLaunch launch = {(uint8_t)rogue, (uint8_t)is_right_side(rogue),
                               PROGRAM_ROGUE_FIGHTER};
        launch_diver(world, &launch, launch_bomb_bits(inputs));
    }
}

/**
 * `case_bmbr_boss`: when no capture is going on, one Boss launch in two is a capture: the first
 * resting Boss dives alone along the capture program. With no Boss resting, nothing leaves.
 */
static void launch_boss_or_capture(AttackWorld *world, const AttackInputs *inputs)
{
    if (world->capture->busy) {
        launch_boss(world, inputs);
        return;
    }
    world->attacks->boss_toggle++;
    if ((world->attacks->boss_toggle & 1) != 0) {
        launch_boss(world, inputs);
        return;
    }
    int32_t boss = first_at_home(world->objects, FIRST_BOSS);
    if (boss < 0)
        return;
    world->capture->busy = 1;
    world->capture->object = (uint8_t)boss;
    queue_boss(world, boss, 0, 0, PROGRAM_CAPTURE);
}

/** `l_1B8B`: the next queued launch, if the alien is still at home. */
static void launch_queued(AttackWorld *world, const AttackInputs *inputs)
{
    Attacks *attacks = world->attacks;
    QueuedLaunch next = attacks->queue[0];
    for (int32_t i = 1; i < attacks->queue_length; i++)
        attacks->queue[i - 1] = attacks->queue[i];
    attacks->queue_length--;
    if (at_home(world->objects, next.object))
        launch_diver(world, &next, launch_bomb_bits(inputs));
}

/** `l_1BA8`: the countdowns step; the first to run out launches its kind of alien. */
static void count_down(AttackWorld *world, const AttackInputs *inputs)
{
    uint8_t *countdown = world->attacks->countdown;
    int32_t expired = countdown[0] == 1 ? 0 : countdown[1] == 1 ? 1 : countdown[2] == 1 ? 2 : -1;
    /* Countdowns step in order up to the first that runs out, like the original loop. */
    int32_t last = expired < 0 ? 2 : expired;
    for (int32_t i = 0; i <= last; i++)
        countdown[i] = (uint8_t)(countdown[i] - 1);
    if (expired < 0)
        return;
    if (inputs->flying >= inputs->settings.max_bombers) {
        countdown[expired] = 1;
        return;
    }
    countdown[expired] = (uint8_t)inputs->settings.reload[expired];
    switch (expired) {
    case 0:
        launch_boss_or_capture(world, inputs);
        return;
    case 1:
        launch_first(world, 0x40, 16, PROGRAM_BUTTERFLY, inputs);
        return;
    default:
        launch_first(world, 0x08, 20, PROGRAM_BEE, inputs);
        return;
    }
}

void run_attacks(AttackWorld *world, const AttackInputs *inputs)
{
    if (inputs->enemies_enabled && (!inputs->fire_enabled || inputs->rescue_running))
        return;
    if (world->attacks->queue_length > 0)
        launch_queued(world, inputs);
    else if ((inputs->frame & 0x0f) == 0)
        count_down(world, inputs);
}
