/* SPDX-License-Identifier: GPL-3.0-only */
#include "records.h"
#include "test/check.h"

static void a_game_without_a_record_has_zero(void)
{
    const Records records = records_empty();
    CHECK_EQ(records_get(&records, 0x494E5644u), 0);
    CHECK(records_valid(&records));
}

static void records_are_found_by_their_tag(void)
{
    Records records = records_empty();
    records = records_set(&records, 1u, 1500);
    records = records_set(&records, 2u, 300);
    records = records_set(&records, 1u, 2000);
    CHECK_EQ(records.count, 2);
    CHECK_EQ(records_get(&records, 1u), 2000);
    CHECK_EQ(records_get(&records, 2u), 300);
}

static void a_full_list_takes_no_new_game(void)
{
    Records records = records_empty();
    for (uint32_t tag = 1; tag <= RECORDS_MAX + 1u; tag++)
        records = records_set(&records, tag, 10);
    CHECK_EQ(records.count, RECORDS_MAX);
    CHECK_EQ(records_get(&records, RECORDS_MAX + 1u), 0);
}

static void rubbish_from_the_flash_is_refused(void)
{
    Records records = records_empty();
    records.version = 1u;
    CHECK(!records_valid(&records));
    records = records_empty();
    records.count = RECORDS_MAX + 1u;
    CHECK(!records_valid(&records));
    records = records_empty();
    records = records_set(&records, 1u, 0);
    records.list[0].score = -5;
    CHECK(!records_valid(&records));
}

static void equal_records_compare_equal(void)
{
    const Records a = records_set(&(Records){RECORDS_VERSION, 0u, {{0u, 0}}}, 1u, 50);
    Records b = records_empty();
    CHECK(!records_equal(&a, &b));
    b = records_set(&b, 1u, 50);
    CHECK(records_equal(&a, &b));
}

int main(void)
{
    RUN(a_game_without_a_record_has_zero);
    RUN(records_are_found_by_their_tag);
    RUN(a_full_list_takes_no_new_game);
    RUN(rubbish_from_the_flash_is_refused);
    RUN(equal_records_compare_equal);
    return check_report();
}
