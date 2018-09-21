#include <semaphore.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "clock.h"

char *buffer;
// sem_t* empty;
// sem_t* full;
// sem_t* mutex;
typedef struct 
{
    sem_t sem;
} shared;

shared* empty;
shared* full;
shared* mutex;
int mesg_size;
int buff_size = 5;

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
        char *tmp = malloc(mesg_size);
        sem_wait(&full->sem); // Line C1
        sem_wait(&mutex->sem); // Line C1.5 (MOVED MUTEX HERE...)
        // int tmp = get(); // Line C2
        memcpy(tmp, buffer, mesg_size);
        sem_post(&mutex->sem); // Line C2.5 (... AND HERE)
        sem_post(&empty->sem); // Line C3
        _exit(EXIT_SUCCESS);
    }
    else
    {
        // parent producer
        struct timespec start, end, *diff;
        char *tmp = malloc(mesg_size);
        sem_wait(&empty->sem); // Line P1
        sem_wait(&mutex->sem); // Line P1.5 (MOVED MUTEX HERE...)
        clock_gettime(CLOCK_MONOTONIC, &start);  
        memcpy(buffer, tmp, mesg_size);
        sem_post(&mutex->sem); // Line P2.5 (... AND HERE)
        sem_post(&full->sem); // Line P3
        clock_gettime(CLOCK_MONOTONIC, &end);
        sem_wait(&empty->sem);
        diff = get_diff(&end, &start);
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        return (double)elapsed;
    }
}

double throughput(int mesg_size, int total_size)
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
        char *tmp = malloc(mesg_size);
        int received = 0;
        // buffer[0] = 'f';
        // buffer[1] = 'u';
        // buffer[2] = 'c';
        // buffer[3] = 'k';
        // buffer[4] = '\0';
        while(received < total_size)
        {
            // printf("fuck you!%d\n", received);
            sem_wait(&full->sem); // Line C1
            // printf("fuck you!%d\n", received);
            sem_wait(&mutex->sem); // Line C1.5 (MOVED MUTEX HERE...)
            // printf("fuck you!%d\n", received);
            // int tmp = get(); // Line C2
            memcpy(tmp, buffer, mesg_size);
            // printf("fuck you!%d\n", received);
            sem_post(&mutex->sem); // Line C2.5 (... AND HERE)
            // printf("fuck you!%d\n", received);
            sem_post(&empty->sem); // Line C3
            // printf("fuck you!%d\n", received);
            received += mesg_size;
        }
        _exit(EXIT_SUCCESS);
    }
    else
    {
        // parent producer
        struct timespec start, end, *diff;
        char *tmp = malloc(mesg_size);
        int sent = 0;
        int offset = 0;
        // sleep(3);
        // printf("%s\n", buffer);
        clock_gettime(CLOCK_MONOTONIC, &start);
        while(sent < total_size)
        {
            sem_wait(&empty->sem); // Line P1
            sem_wait(&mutex->sem); // Line P1.5 (MOVED MUTEX HERE...)
            memcpy(buffer + offset * mesg_size, tmp, mesg_size);
            sem_post(&mutex->sem); // Line P2.5 (... AND HERE)
            sem_post(&full->sem); // Line P3
            sent += mesg_size;
            offset = (offset + 1) % buff_size;
            // printf("%d\n", sent);
        }
        sem_wait(&empty->sem);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_diff(&end, &start);
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        return (double) total_size / (double) elapsed * (1000000000.0 /1024.0/1024.0);
    }
}
int main(int argc, char *argv[]) {
    mesg_size = atoi(argv[1]);
    empty = (shared *)mmap(NULL, sizeof(shared), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    full = (shared *)mmap(NULL, sizeof(shared), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mutex = (shared *)mmap(NULL, sizeof(shared), PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    buffer = (char *)mmap(NULL, mesg_size * buff_size, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    sem_init(&empty->sem, 1, buff_size); // MAX buffers are empty to begin with...
    sem_init(&full->sem, 1, 0); // ... and 0 are full
    sem_init(&mutex->sem, 1, 1); // mutex=1 because it is a lock

    if (argc > 2)
    {
        double t = throughput(mesg_size, atoi(argv[2]));
        printf("%f\n", t);
    }
    else
    {
        double t = latency(mesg_size);
        printf("%f\n", t/1000.0);
    }

}
