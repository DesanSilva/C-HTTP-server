#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <openssl/ssl.h>

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

int socket_create(void);
void socket_bind(int sockfd, int port);
void socket_listen(int sockfd);
int socket_connect(int sockfd, struct sockaddr_in *clientAddr);

SSL_CTX* ssl_initialize(void);
int      ssl_load_certificates(SSL_CTX *ctx, const char *certificate, const char *privateKey);
SSL*     ssl_connect(SSL_CTX *ctx, int clientfd);
void     ssl_cleanup(SSL *ssl, SSL_CTX *ctx);
void handle_client(ClientConnection *conn);

#endif
