/* SPDX-License-Identifier: GPL-3.0-only */
#include "wave_launcher.h"

#include "attacks.h"
#include "rom.h"
#include "table.h"

void start_launcher(Launcher *launcher, const StageWaves *waves)
{
    for (int32_t i = 0; i < waves->count; i++)
        launcher->tokens[i] = waves->tokens[i];
    launcher->count = waves->count;
    launcher->next = 0;
    launcher->wave_count = 0;
    launcher->running = 1;
}

Launcher idle_launcher(void)
{
    Launcher launcher;
    launcher.count = 0;
    launcher.next = 0;
    launcher.wave_count = 0;
    launcher.running = 0;
    return launcher;
}

/** The look of a special enemy: butterfly, bee, or Boss in the third wave (`l_29B3`). */
static AlienLook transient_look(int32_t id, int32_t wave_count)
{
    AlienLook look = {0x18, 3, 0};
    if ((id & 0x40) != 0) {
        look.code = 0x10;
        look.color = 2;
    } else if (wave_count == 2) {
        look.code = 0x08;
        look.color = 0;
    }
    return look;
}

/** `l_29D1`: the slot of an alien entering with control byte `control`. */
static FlightSlot entry_slot(int32_t control, int32_t object, int32_t bomb_bits)
{
    int32_t program = TABLE(ENTRY_PROGRAMS, (control & 0x3f) * 2);
    int32_t high = TABLE(ENTRY_PROGRAMS, (control & 0x3f) * 2 + 1);
    int mirrored = (control & 0x40) != 0;
    int32_t start = (high >> 5) * 6 + (mirrored ? 3 : 0);
    FlightSlot slot = new_slot(object);
    slot.y = (uint16_t)(TABLE(ENTRY_STARTS, start) << 8);
    slot.x = (uint16_t)(TABLE(ENTRY_STARTS, start + 1) << 8);
    slot.angle = (uint16_t)(TABLE(ENTRY_STARTS, start + 2) << 8);
    slot.pointer = (uint16_t)(program | ((high & 0x1f) << 8));
    slot.bomb_timer = (control & 1) == 1 ? 0x44 : 0x08;
    slot.bomb_bits = (uint8_t)bomb_bits;
    slot.mirrored = (uint8_t)mirrored;
    return slot;
}

/** `l_295E`: puts the next alien in the first free slot, or waits if none is free. */
static void launch(Launcher *launcher, Slots *slots, Objects *objects, const WaveToken *entry,
                   const LauncherInputs *inputs)
{
    int32_t free = first_free_slot(slots);
    if (free < 0)
        return;
    /* Objects $78-$7E stand for the special enemies $38-$3E that are butterflies. */
    int32_t object = (entry->object & 0x78) == 0x78 ? entry->object & ~0x40 : entry->object;
    AlienLook look = {0x18, 3, 0};
    if ((object & 0x38) == 0x38)
        look = transient_look(entry->object, launcher->wave_count);
    else if (object_exists(object) && inputs->looks->known[object >> 1])
        look = inputs->looks->look[object >> 1];
    slots->list[free] =
        entry_slot(entry->control, object, look.bombs ? inputs->entry_bomb_bits : 0);
    launcher->next++;
    GameObject *alien = object_ref(objects, object);
    if (!alien)
        return;
    alien->disposition = DISPOSITION_ENTERING;
    alien->counter = (uint8_t)free;
    alien->code = look.code;
    alien->color = look.color;
    alien->flip_x = 0;
    alien->flip_y = 0;
    alien->big = 0;
    alien->hit = 0;
}

/**
 * `l_2944`: a start mark. The next wave waits until nothing flies; in a challenging stage it also
 * waits for the pause of game timer 0 (`f_2916`).
 */
static void start_wave(Launcher *launcher, const LauncherInputs *inputs, LauncherReport *report)
{
    if (!inputs->enabled)
        return;
    if (inputs->flying != 0) {
        report->timer = 2;
        return;
    }
    if (inputs->challenge && inputs->timer == 1) {
        report->group_reset = 1;
        return;
    }
    if (inputs->challenge && inputs->timer != 0)
        return;
    launcher->next++;
    launcher->wave_count++;
}

LauncherReport run_launcher(Launcher *launcher, Slots *slots, Objects *objects,
                            const LauncherInputs *inputs)
{
    LauncherReport report = {-1, 0, 0};
    if (!launcher->running)
        return report;
    const WaveToken *token =
        launcher->next < launcher->count ? &launcher->tokens[launcher->next] : 0;
    if (!token || token->kind == TOKEN_END) {
        if (inputs->flying == 0) {
            launcher->running = 0;
            report.completed = 1;
        }
        return report;
    }
    if (token->kind == TOKEN_START) {
        start_wave(launcher, inputs, &report);
        return report;
    }
    /* Without bit 7, the alien follows the previous one in line: one every 8 frames. */
    if ((token->control & 0x80) == 0 && (inputs->frame & 7) != 0)
        return report;
    launch(launcher, slots, objects, token, inputs);
    return report;
}
