/* SPDX-License-Identifier: GPL-3.0-only */
#include "test/check.h"
#include "trace/text.h"

#include <string.h>

static Text text;

static void strings_and_numbers_are_appended(void)
{
    text_clear(&text);
    text_add(&text, "s=");
    text_add_int(&text, 650);
    text_add_char(&text, ';');
    text_add_int(&text, -1);
    text_add_char(&text, ';');
    text_add_int(&text, 0);
    CHECK(strcmp(text_cstr(&text), "s=650;-1;0") == 0);
}

static void the_smallest_number_has_its_digits(void)
{
    text_clear(&text);
    text_add_int(&text, -2147483647 - 1);
    CHECK(strcmp(text_cstr(&text), "-2147483648") == 0);
}

static void hex_keeps_its_leading_zeros(void)
{
    text_clear(&text);
    text_add_hex(&text, 0x0FFFCu, 6u);
    CHECK(strcmp(text_cstr(&text), "00fffc") == 0);
}

static void too_much_text_is_cut_and_flagged(void)
{
    text_clear(&text);
    for (uint32_t i = 0; i < TEXT_CAPACITY + 10u; i++)
        text_add_char(&text, 'x');
    CHECK(text.overflow);
    CHECK_EQ(text.length, TEXT_CAPACITY - 1u);
}

static void the_hash_matches_the_typescript_recorder(void)
{
    /* the attract screen before any game: "# initial e8c203ae" in every trace */
    text_clear(&text);
    text_add(&text, "A|hi=0");
    CHECK_EQ(text_fnv1a(&text), 0xE8C203AEu);
}

int main(void)
{
    RUN(strings_and_numbers_are_appended);
    RUN(the_smallest_number_has_its_digits);
    RUN(hex_keeps_its_leading_zeros);
    RUN(too_much_text_is_cut_and_flagged);
    RUN(the_hash_matches_the_typescript_recorder);
    return check_report();
}
