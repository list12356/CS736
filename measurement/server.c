#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <netinet/tcp.h>

#define PORT 8080
#define BUFFSIZE 1024*1024*2

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char *mesg = "ack"; 
    int mesg_size = atoi(argv[1]);
    int total;
    char *buffer = malloc(sizeof(char) * mesg_size); 

    if (argc > 2)
        total = atoi(argv[2]);
    else
        total = mesg_size;

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                        &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    // while(1)
    // {
    int num = 1, received = 0;
    // while
    while(num > 0 && received < total)
    {
        // num = recv(new_socket , buffer, mesg_size, 0);
        num = read(new_socket, buffer, mesg_size);
        received += num;
        // buffer += num;
        // printf("%d\n", received);
    }
    send(new_socket , mesg , strlen(mesg) , 0 ); 
    return 0; 
} 
