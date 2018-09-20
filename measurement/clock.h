
#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <stdint.h>
#include <time.h>

static uint64_t inline get_tick()
{
    uint64_t cycles_low, cycles_high;
    __asm__ volatile("CPUID\n\t"
                    "rdtsc" : "=a" (cycles_low), "=d" (cycles_high));
    return ( ((uint64_t)cycles_high << 32) | cycles_low );
}
extern double frequency;

struct timespec *get_diff(struct timespec *ts1, struct timespec *ts2);
double get_frequency();
double get_resolution_rdtsc();
double get_resolution_clock_time();
double get_resolution_time_of_day();
void bind_cpu();
void clock_init();

#endif