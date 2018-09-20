#include <sched.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include "clock.hpp"

#define SIZE_OF_STAT 100000
#define BOUND_OF_LOOP 1000
// #define UINT64_MAX (18446744073709551615ULL)

using namespace std;

static void inline test_func()
{
    int tmp = 0;
}

static uint64_t inline hrclock::get_tick()
{
    uint64_t cycles_low, cycles_high;
    __asm__ volatile("rdtsc" : "=a" (cycles_low), "=d" (cycles_high));
    return ( ((uint64_t)cycles_high << 32) | cycles_low );
}

void hrclock::bind_cpu()
{
    cpu_set_t my_set;        /* Define your cpu_set bit mask. */
    CPU_ZERO(&my_set);       /* Initialize it all to 0, i.e. no CPUs selected. */
    CPU_SET(1, &my_set);     /* set the bit that represents core 7. */
    sched_setaffinity(0, sizeof(cpu_set_t), &my_set);
}

double hrclock::get_resolution_clock_time()
{
    struct timespec start, end, *diff;
    int try_num = 10000;
    double best = 10000;
    while(try_num--)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);
        test_func();
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_diff(&end, &start);
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        if (elapsed > 0 && elapsed < best)
            best = elapsed;
    }
    return best;
}

double hrclock::get_resolution_rdtsc()
{
    int start, end, best = 10000, elapsed = 0;
    int try_num = 10000;
    while(try_num--)
    {
        start = get_tick();
        test_func();
        end = get_tick();
        if ((end - start) > 0 && (end - start) < best)
            best = end - start;
    }
    return (double) best / frequency;
}
double hrclock::frequency;

double hrclock::get_frequency()
{
    struct timespec start, end;
    int start_tick, end_tick;
    clock_gettime(CLOCK_MONOTONIC, &start);
    start_tick = get_tick();
    sleep(3);
    end_tick = get_tick();
    clock_gettime(CLOCK_MONOTONIC, &end);
    struct timespec *diff = get_diff(&end, &start);
    uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
    frequency = (double)(end_tick - start_tick)/(double)elapsed;
}

struct timespec* hrclock::get_diff(struct timespec *ts1, struct timespec *ts2)
{
    static struct timespec ts;
    ts.tv_sec = ts1->tv_sec - ts2->tv_sec;
    ts.tv_nsec = ts1->tv_nsec - ts2->tv_nsec;
    if (ts.tv_nsec < 0) {
        ts.tv_sec--;
        ts.tv_nsec += 1000000000LL;
    }
    return &ts;
}

void hrclock::clock_init()
{
    bind_cpu();
    get_frequency();
}

int main(int argc, char* argv[])
{
    using namespace hrclock;
    clock_init();
    cout << "frequency: " << frequency << endl;
    cout << get_resolution_rdtsc() << endl;
    cout << get_resolution_clock_time() << endl;
    return 0;
}