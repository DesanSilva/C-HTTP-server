#ifndef TYPES_H
#define TYPES_H

#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 4096
#define RESPONSE_BUFFER_SIZE 8192

typedef struct ServerSocket {
    int sockfd;
    struct sockaddr_in address;
    int port;
} ServerSocket;

typedef struct SSLConfig {
    SSL_CTX *ctx;
    const char *certificate;
    const char *privateKey;
} SSLConfig;

typedef struct ClientConnection {
    int clientfd;
    SSL *ssl;
    struct sockaddr_in clientAddr;
} ClientConnection;

#endif
