#ifndef METHODS_H
#define METHODS_H

#define DEFAULT_HTTPS_PORT 8443

void serve(int port);

int socket_create(void);
void socket_bind(int sockfd, int port);
void socket_listen(int sockfd);
int socket_connect(int sockfd, struct sockaddr_in *clientAddr);

SSL_CTX* ssl_initialize(void);
int      ssl_load_certificates(SSL_CTX *ctx, const char *certificate, const char *privateKey);
SSL*     ssl_connect(SSL_CTX *ctx, int clientfd);
void     ssl_cleanup(SSL *ssl, SSL_CTX *ctx);

void handle_client(ClientConnection *conn);
void respond(SSL *ssl, const char *status, const char *contentType, char *body);

char* read_file(char* fileName);
void file_log(LogType type, char* message, ...);

#endif
