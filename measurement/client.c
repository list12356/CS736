// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include "clock.h"
#define PORT 8080

#define BUFFSIZE 1024*1024*2
   
double throughput(int mesg_size, int total_size)
{
    struct sockaddr_in address; 
    int sock = 0, valread, rtn;
    struct sockaddr_in serv_addr; 
    char *message = malloc(sizeof(char)*mesg_size); 
    char buffer[BUFFSIZE] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    struct timespec start, end, *diff;
    int sent_byte = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);    

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    
    while(sent_byte <  total_size)
    {
        rtn = send(sock , message , mesg_size , 0 );
        // if (rtn == -1)
        // {
        //     printf("Error when sending message\n");
        //     close(sock);
        //     return 0;
        // }
        sent_byte += rtn;
    }
    valread = read( sock , buffer, BUFFSIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = get_diff(&end, &start);
    uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
    double rate  = (double) total_size / (double) elapsed * (1000000000.0 / 1024.0/1024.0);
    printf("%f\n", rate);
    return rate;
}

double latency(int mesg_size)
{
    struct sockaddr_in address; 
    int sock = 0, valread, rtn;
    struct sockaddr_in serv_addr; 
    char *message = malloc(sizeof(char)*mesg_size); 
    char buffer[BUFFSIZE] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&serv_addr, '0', sizeof(serv_addr)); 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    struct timespec start, end, *diff;
    clock_gettime(CLOCK_MONOTONIC, &start);    

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    // int start = get_tick();
    rtn = send(sock , message , mesg_size , 0 );
    if (rtn == -1)
    {
        printf("Error when sending message\n");
        close(sock);
        return 0;
    }
    valread = read( sock , buffer, BUFFSIZE); 
    // int end = get_tick();
    clock_gettime(CLOCK_MONOTONIC, &end);
    diff = get_diff(&end, &start);
    uint64_t elapsed = diff->tv_sec * 1000000000LL + diff->tv_nsec;
    double rate  = (double) elapsed / 1000.0;
    printf("%f\n", rate);
    return rate;
}

int main(int argc, char const *argv[]) 
{ 
    // clock_init();
    // throughput(128);
    int mesg_size = atoi(argv[1]);
    // int opt = atoi(argv[2]);
    if (argc > 2)
        throughput(mesg_size, atoi(argv[2]));
    else 
        latency(mesg_size);
    return 0;
} 
