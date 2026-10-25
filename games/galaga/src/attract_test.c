/* SPDX-License-Identifier: GPL-3.0-only */
#include <string.h>

#include "attract.h"
#include "test/check.h"

static Attract attract;

/** The attract screen after `frames` frames. */
static const Attract *after(int32_t frames)
{
    new_attract(&attract);
    for (int32_t i = 0; i < frames; i++)
        update_attract(&attract);
    return &attract;
}

static void the_info_screen_is_written_a_line_about_every_second(void)
{
    CHECK_EQ(after(30)->messages.count, 0);
    CHECK_EQ(after(70)->messages.count, 1);
    CHECK(strcmp(attract.messages.list[0].text, "GALAGA") == 0);
    CHECK_EQ(after(200)->messages.count, 3);
    CHECK(strcmp(attract.messages.list[1].text, "-- SCORE --") == 0);
    CHECK(strcmp(attract.messages.list[2].text, "50    100") == 0);
}

static void the_aliens_of_the_score_table_show_upright_from_the_third_line(void)
{
    const Attract *shown = after(330);
    const GameObject *boss = object_at(&shown->objects, 0x08);
    const GameObject *butterfly = object_at(&shown->objects, 0x0c);
    CHECK_EQ(boss->code, 0x1e);
    CHECK_EQ(boss->color, 3);
    CHECK_EQ(boss->x, 0x44);
    CHECK_EQ(boss->y, 0x74);
    CHECK_EQ(butterfly->code, 0x0e);
    CHECK_EQ(butterfly->x, 0x7c);
    CHECK_EQ(butterfly->y, 0xa0);
}

static void it_ends_with_the_copyright_then_starts_again(void)
{
    const Attract *copyright = after(400);
    CHECK_EQ(copyright->step, ATTRACT_COPYRIGHT);
    CHECK(strcmp(copyright->messages.list[copyright->messages.count - 1].text,
                 "\xa9 1981 NAMCO LTD.") == 0);
    CHECK_EQ(after(700)->step, ATTRACT_INFO);
    CHECK_EQ(attract.messages.count, 0);
}

int main(void)
{
    RUN(the_info_screen_is_written_a_line_about_every_second);
    RUN(the_aliens_of_the_score_table_show_upright_from_the_third_line);
    RUN(it_ends_with_the_copyright_then_starts_again);
    return check_report();
}
