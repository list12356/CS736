#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "clock.h"

#define BUFFSIZE 128

double throughput()
{
    return 0;
}

int main(int argc, char* argv[])
{
    int mesg_size = 128 * 1024 * 1024;
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
        char *buff = malloc(sizeof(char) * BUFFSIZE);
        while (read(pipefd_send[0], buff, BUFFSIZE) > 0);
        close(pipefd_send[0]);
        write(pipefd_ack[1], "ack", 4);
        close(pipefd_ack[1]);
        _exit(EXIT_SUCCESS);
    } else {            /* Parent writes argv[1] to pipe */
        close(pipefd_send[0]);          /* Close unused read end */
        close(pipefd_ack[1]);
        char *ack = malloc(sizeof(char) * 4);
        clock_init();

        int start = get_tick();
        write(pipefd_send[1], mesg, strlen(mesg));
        close(pipefd_send[1]);          /* Reader will see EOF */
        while (read(pipefd_ack[0], ack, 4) == 0);
        int end = get_tick();
        double rate  = 128.0 / ((double) (end - start) / frequency / 1000000000LL) / 1024;
        printf("transmission rate:%f \n", rate);
        close(pipefd_ack[0]);
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}