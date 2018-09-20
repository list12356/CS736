#include "clock.h"

int main(int argc, char* argv[])
{
    clock_init();
    printf("frequency: %f\n", frequency);
    for(int i = 0; i < 100; i++)
    {
        printf("%f\t", get_resolution_rdtsc());
        printf("%f\t", get_resolution_clock_time());
        printf("%f\n", get_resolution_time_of_day());
    }
    return 0;
}