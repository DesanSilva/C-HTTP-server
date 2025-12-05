#ifndef REQUESTS_H
#define REQUESTS_H

#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096
#define RESPONSE_BUFFER_SIZE 8192

void respond(SSL *ssl, const char *status, const char *contentType, char *body);

char* read_file(char* fileName);

#endif
