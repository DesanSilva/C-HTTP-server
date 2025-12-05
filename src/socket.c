#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../include/errors.h"

#define BACKLOG 10

int socket_create() {
    FileError err = ERR_NONE;
    int sockfd;
    int opt = 1;

    // create TCP socket, AF_INET: IPv4 protocol
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        file_log(ERROR, "Socket creation failed");
        err = ERR_SOCK_CREATION_FAIL;
        goto handle_errors;
    }

    // set socket options, SO_REUSEADDR allows binding to an address
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        file_log(ERROR, "Setsockopt failed");
        err = ERR_SETSOCKOPT_FAIL;
        goto handle_errors;
    }

    file_log(INFO, "Socket created successfully");
    return sockfd;

handle_errors:
    switch (err) {
        case ERR_SETSOCKOPT_FAIL: close(sockfd);
        case ERR_SOCK_CREATION_FAIL:
        default: return -1;
    }
}

void socket_bind(int sockfd, int port) {
    struct sockaddr_in address = {0};

    // set address family
    address.sin_family = AF_INET;
    // bind to all available interfaces
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    // convert port to network byte order
    address.sin_port = htons(port);

    // attempt to bind socket to the specified port
    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        file_log(ERROR, "Socket failed to bind");
        goto handle_errors;
    }
    
    // bind successful
    file_log(DEBUG, "Socket bound to port %d", port);
    return;

handle_errors:
    // only error: Socket bind fail error
    close(sockfd);
}

void socket_listen(int sockfd) {
    // convert TCP socket to a passive socket and listen for incoming connections
    if (listen(sockfd, BACKLOG) < 0) {
        file_log(ERROR, "Socket listen failed");
        goto handle_errors;
    }

    file_log(INFO, "Listening for connections");
    return;

handle_errors:
    // only error: listen error
    close(sockfd);
}

int socket_connect(int sockfd, struct sockaddr_in *clientAddr) {
    socklen_t addrLen = sizeof(*clientAddr);
    int clientfd;

    // accept an incoming client connection
    clientfd = accept(sockfd, (struct sockaddr *)clientAddr, &addrLen);
    if (clientfd < 0) {
        file_log(ERROR, "Failed to accept connection from client");
        goto handle_errors;
    }

    // extract and log client information
    char* clientAddress = inet_ntoa(clientAddr -> sin_addr);
    int clientPort = ntohs(clientAddr -> sin_port);

    file_log(DEBUG, "connection accepted from %s:%d", clientAddress, clientPort);
    return clientfd;

handle_errors:
    // only error: client accept failed
    return -1;
}
