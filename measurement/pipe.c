#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "clock.h"
#include "pipe.h"


#define BUFFSIZE 1024*1024

double throughput(int mesg_size)
{
    char* mesg = malloc(sizeof(char) * mesg_size);
    int pipefd_send[2];
    int pipefd_ack[2];
    double rate;
    if (pipe(pipefd_send) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd_ack) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    int rc = fork();

    if (rc == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (rc == 0) {    /* Child reads from pipe */
        close(pipefd_send[1]);          /* Close unused write end */
        close(pipefd_ack[0]);
        char *buff = malloc(sizeof(char) * mesg_size);
        int nread;
        nread = read(pipefd_send[0], buff, mesg_size);
        close(pipefd_send[0]);
        write(pipefd_ack[1], "ack", 4);
        close(pipefd_ack[1]);
        _exit(EXIT_SUCCESS);
    } else {
        close(pipefd_send[0]);          /* Close unused read end */
        close(pipefd_ack[1]);
        char *ack = malloc(sizeof(char) * 4);
        // clock_init();

        // int start = get_tick();
        struct timespec start, end, *diff;
        clock_gettime(CLOCK_MONOTONIC, &start);
        write(pipefd_send[1], mesg, mesg_size);
        close(pipefd_send[1]);          /* Reader will see EOF */
        read(pipefd_ack[0], ack, 4);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_diff(&end, &start);
        // int end = get_tick();
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        rate  = mesg_size / ((double) elapsed / 1000000000LL) / 1024 / 1024;
        // printf("transmission rate:%f MB/s\n", rate);
        close(pipefd_ack[0]);
        wait(NULL);                /* Wait for child */
        // exit(EXIT_SUCCESS);
    }
    return rate;
}

double latency(int mesg_size)
{
    double rate = 0;
    char* mesg = malloc(sizeof(char) * mesg_size);
    int pipefd_send[2];
    int pipefd_ack[2];
    if (pipe(pipefd_send) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd_ack) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    
    int rc = fork();

    if (rc == -1) 
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (rc == 0) {    /* Child reads from pipe */
        close(pipefd_send[1]);          /* Close unused write end */
        close(pipefd_ack[0]);
        char *buff = malloc(sizeof(char) * mesg_size);
        read(pipefd_send[0], buff, mesg_size);
        close(pipefd_send[0]);
        write(pipefd_ack[1], "ack", 4);
        close(pipefd_ack[1]);
        _exit(EXIT_SUCCESS);
    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd_send[0]);          /* Close unused read end */
        close(pipefd_ack[1]);
        char *ack = malloc(sizeof(char) * 4);
        // int start = get_tick();
        struct timespec start, end, *diff;
        clock_gettime(CLOCK_MONOTONIC, &start);
        write(pipefd_send[1], mesg, mesg_size);
        close(pipefd_send[1]);          /* Reader will see EOF */
        read(pipefd_ack[0], ack, 4);
        clock_gettime(CLOCK_MONOTONIC, &end);
        diff = get_diff(&end, &start);
        // int end = get_tick();
        uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
        rate  = ((double) elapsed );
        // printf("latency:%f us\n", rate /1000.0);
        close(pipefd_ack[0]);
        wait(NULL);                /* Wait for child */
        // exit(EXIT_SUCCESS);
    }
    return rate / 2.0 / 1000.0;
}

int main(int argc, char* argv[])
{
    // clock_init();
    // printf("%f\n", frequency);
    double a, b;
    a = latency(4);
    b = throughput(4);
   printf("%f\t%f\n", a, b);

    a = latency(16);
    b = throughput(16);
    printf("%f\t%f\n", a, b);
    
    a = latency(64);
    b = throughput(64);
    printf("%f\t%f\n", a, b);

    a = latency(256);
    b = throughput(256);
    printf("%f\t%f\n", a, b);

    a = latency(1024);
    b = throughput(1024);
    printf("%f\t%f\n", a, b);

    a = latency(4*1024);
    b = throughput(4*1024);
    printf("%f\t%f\n", a, b);

    a = latency(16*1024);
    b = throughput(16*1024);
    printf("%f\t%f\n", a, b);

    a = latency(64*1024);
    b = throughput(64*1024);
    printf("%f\t%f\n", a, b);


    a = latency(256*1024);
    b = throughput(256*1024);
    printf("%f\t%f\n", a, b);

    a = latency(512*1024);
    b = throughput(512*1024);
    printf("%f\t%f\n", a, b);

    b = throughput(4*1024*1024);
    printf("%f\t%f\n", a, b);

    b = throughput(16*1024*1024);
    printf("%f\t%f\n", a, b);

    b = throughput(64*1024*1024);
    printf("%f\t%f\n", a, b);


    b = throughput(256*1024*1024);
    printf("%f\t%f\n", a, b);

    b = throughput(1024*1024*1024);
    printf("%f\t%f\n", a, b);
}
