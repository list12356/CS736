#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "clock.h"

sem_t empty;
sem_t full;
sem_t mutex;

char *buffer;
int buff_size;

double latency(int mesg_size)
{
    int rc = fork();

    if (rc == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (rc == 0)
    {
        // Child consumer
        char *tmp = malloc(buff_size);
        sem_wait(&full); // Line C1
        sem_wait(&mutex); // Line C1.5 (MOVED MUTEX HERE...)
        // int tmp = get(); // Line C2
        memcpy(tmp, buffer, buff_size);
        sem_post(&mutex); // Line C2.5 (... AND HERE)
        sem_post(&empty); // Line C3
        _exit(EXIT_SUCCESS);
    }
    else
    {
        // parent producer
        struct timespec start, end, *diff;
        char *tmp = malloc(buff_size);
        sem_wait(&empty); // Line P1
        sem_wait(&mutex); // Line P1.5 (MOVED MUTEX HERE...)
        clock_gettime(CLOCK_MONOTONIC, &start);  
        memcpy(buffer, tmp, buff_size);
        sem_post(&mutex); // Line P2.5 (... AND HERE)
        sem_post(&full); // Line P3
        sem_wait(&empty);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_diff(&end, &start);
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        return (double)elapsed;
    }
}

int main(int argc, char *argv[]) {
    buff_size = atoi(argv[1]);
    buffer = (char *)mmap(NULL, buff_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(&empty, 0, buff_size); // MAX buffers are empty to begin with...
    sem_init(&full, 0, 0); // ... and 0 are full
    sem_init(&mutex, 0, 1); // mutex=1 because it is a lock

    // if (atoi(argv[2]) == 0)
    // {
        double t = latency(buff_size);
        printf("%f\n", t/1000.0);
    // }
}