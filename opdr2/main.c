#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    // Stringbuffer for messages
    char str[100];

    // fd for server and client
    int sock_fd, comm_fd;
    int opt = 1;

    // socket struct
    struct sockaddr_in server;

    // get a socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // error checking
    if(sock_fd == -1){
        perror("socket");
        return sock_fd;
    }

    // clear the socket struct
    bzero(&server, sizeof(server));

    // force port usage
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    // set socket properties
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(23);

    // bind the socket to a server
    if(bind(sock_fd, (struct sockaddr*) &server, sizeof(server)) < 0){
        perror("bind failed");
        exit(-1);
    }

    // listen for incoming clients
    if(listen(sock_fd, 10) < 0){
        perror("listen failed");
        exit(-1);
    }

    // if we receive a client, keep track of its fd
    if((comm_fd = accept(sock_fd, (struct sockaddr*) NULL, NULL)) < 0){
        perror("client failed");
        exit(-1);
    }

    // print received messages
    while(1){
        bzero(str, 100);

        read(comm_fd, str, 100);

        printf("Received: %s", str);
    }
}