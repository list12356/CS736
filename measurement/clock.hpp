
#ifndef __CLOCK_H__
#define __CLOCK_H__

namespace hrclock
{

static int inline get_tick();

extern double frequency;

struct timespec *get_diff(struct timespec *ts1, struct timespec *ts2);
double get_frequency();
double get_resolution_rdtsc();
double get_resolution_clock_time();
void bind_cpu();
void clock_init();

}

#endif