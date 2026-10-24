/* SPDX-License-Identifier: GPL-3.0-only */
/**
 * @file
 * The smallest test harness that does the job: a test file is a program whose main() runs its
 * tests with RUN() and returns check_report(). A failed CHECK prints where and why, and the test
 * goes on, so one run shows every failure. `make test` builds and runs every *_test.c file.
 */
#ifndef CHECK_H
#define CHECK_H

#include <stdio.h>

static int check_failures;
static int check_count;

/** Fails the current test, without stopping it, when `cond` is false. */
#define CHECK(cond)                                                                                \
    do {                                                                                           \
        check_count++;                                                                             \
        if (!(cond)) {                                                                             \
            check_failures++;                                                                      \
            printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                               \
        }                                                                                          \
    } while (0)

/** Fails the current test when two integers differ, printing both. */
#define CHECK_EQ(actual, expected)                                                                 \
    do {                                                                                           \
        const long long check_a = (long long)(actual);                                             \
        const long long check_e = (long long)(expected);                                           \
        check_count++;                                                                             \
        if (check_a != check_e) {                                                                  \
            check_failures++;                                                                      \
            printf("  FAIL %s:%d: %s is %lld, expected %lld\n", __FILE__, __LINE__, #actual,       \
                   check_a, check_e);                                                              \
        }                                                                                          \
    } while (0)

/** Runs one test function and names it in the output. */
#define RUN(test)                                                                                  \
    do {                                                                                           \
        printf("- %s\n", #test);                                                                   \
        test();                                                                                    \
    } while (0)

/** Prints the totals; returns the program's exit code (0 when every check passed). */
static int check_report(void)
{
    printf("%d checks, %d failed\n", check_count, check_failures);
    return check_failures ? 1 : 0;
}

#endif
