/* SPDX-License-Identifier: GPL-3.0-only */
#include "audio/wsg.h"

#include "test/check.h"

/** A PROM whose waveform 0 is a square wave (0 then 15) and the others silent (8). */
static uint8_t prom[WSG_PROM_SIZE];

static void make_prom(void)
{
    for (int i = 0; i < WSG_PROM_SIZE; i++)
        prom[i] = 8;
    for (int i = 0; i < WSG_WAVE_LENGTH; i++)
        prom[i] = i < 16 ? 0 : 15;
}

static void samples_are_centered_on_eight(void)
{
    CHECK_EQ(wsg_sample(prom, 0, 0), -8);
    CHECK_EQ(wsg_sample(prom, 0, 31), 7);
    CHECK_EQ(wsg_sample(prom, 3, 5), 0);
}

static void the_step_is_the_chip_rate_over_ours(void)
{
    /* 96000 / 44100 = 2.1769: in sixteenths, 1024 chip units give about 35,668 */
    CHECK_EQ(wsg_step(1024) >> 4, 2229);
    CHECK_EQ(wsg_step(0), 0);
}

static void a_silent_frame_adds_nothing(void)
{
    Wsg wsg = wsg_new();
    WsgFrame frame = wsg_silent_frame();
    int32_t mix[64] = {0};
    wsg_render(&wsg, &frame, prom, mix, 64);
    for (int i = 0; i < 64; i++)
        CHECK_EQ(mix[i], 0);
}

static void a_voice_plays_its_waveform_at_its_volume(void)
{
    Wsg wsg = wsg_new();
    WsgFrame frame = wsg_silent_frame();
    int32_t mix[400] = {0};
    frame.tick[0][0].frequency = 4410; /* about 404 Hz: a period of 109 samples */
    frame.tick[0][0].volume = 15;
    wsg_render(&wsg, &frame, prom, mix, 400);
    int32_t low = 0, high = 0;
    for (int i = 0; i < 400; i++) {
        low += mix[i] < 0;
        high += mix[i] > 0;
    }
    CHECK(low > 150 && high > 150);
    CHECK_EQ(mix[1], -8 * 15 * 64);
}

static void the_second_tick_follows_the_first(void)
{
    Wsg wsg = wsg_new();
    WsgFrame frame = wsg_silent_frame();
    int32_t mix[800] = {0};
    frame.tick[1][0].frequency = 4410;
    frame.tick[1][0].volume = 15;
    wsg_render(&wsg, &frame, prom, mix, 800);
    CHECK_EQ(mix[100], 0);
    CHECK(mix[500] != 0);
    wsg_start_frame(&wsg);
    CHECK_EQ(wsg.position, 0);
}

static void a_frame_lasts_a_sixtieth_of_a_second(void)
{
    Wsg wsg = wsg_new();
    WsgFrame frame = wsg_silent_frame();
    int32_t mix[1000] = {0};
    CHECK_EQ(wsg_frame_left(&wsg), 735);
    wsg_render(&wsg, &frame, prom, mix, 700);
    CHECK_EQ(wsg_frame_left(&wsg), 35);
    wsg_render(&wsg, &frame, prom, mix, 300); /* past the end: the second tick holds */
    CHECK_EQ(wsg_frame_left(&wsg), 0);
    wsg_start_frame(&wsg);
    CHECK_EQ(wsg_frame_left(&wsg), 735);
}

int main(void)
{
    make_prom();
    RUN(samples_are_centered_on_eight);
    RUN(the_step_is_the_chip_rate_over_ours);
    RUN(a_silent_frame_adds_nothing);
    RUN(a_voice_plays_its_waveform_at_its_volume);
    RUN(the_second_tick_follows_the_first);
    RUN(a_frame_lasts_a_sixtieth_of_a_second);
    return check_report();
}
