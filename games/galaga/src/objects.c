/* SPDX-License-Identifier: GPL-3.0-only */
#include "objects.h"

GameObject no_object(void)
{
    const GameObject object = {DISPOSITION_INACTIVE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return object;
}

Objects empty_objects(void)
{
    Objects objects;
    for (int i = 0; i < OBJECT_COUNT; i++)
        objects.list[i] = no_object();
    return objects;
}

/** What object_at() gives for a number outside the table, as `objects[i] ?? NO_OBJECT`. */
static const GameObject OUTSIDE = {DISPOSITION_INACTIVE, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int object_exists(int32_t id)
{
    return id >= 0 && (id >> 1) < OBJECT_COUNT;
}

const GameObject *object_at(const Objects *objects, int32_t id)
{
    return object_exists(id) ? &objects->list[id >> 1] : &OUTSIDE;
}

GameObject *object_ref(Objects *objects, int32_t id)
{
    return object_exists(id) ? &objects->list[id >> 1] : 0;
}

int is_active(const GameObject *object)
{
    return (object->disposition & DISPOSITION_INACTIVE) == 0;
}
