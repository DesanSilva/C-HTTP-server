#ifndef ERRORS_H
#define ERRORS_H

typedef enum {
    FATAL = 0,
    ERROR,
    WARN,
    INFO,
    DEBUG
} LogType;

typedef enum {
    ERR_NONE = 0,
    ERR_OPEN_FAILED,
    
    // socket.c: socket_create()
    ERR_SOCK_CREATION_FAIL,
    ERR_SETSOCKOPT_FAIL,

    // ssl.c: ssl_initialize()
    ERR_SSL_CONTEXT_FAILED,

    // ssl.c: ssl_load_certificates()
    ERR_CERT_FAIL,
    ERR_PVTKEY_FAIL,
    ERR_CERT_MISMATCH,

    // ssl.c: ssl_connect()
    ERR_SSL_FAIL,
    ERR_SSL_SET_FD_FAIL,
    ERR_SSL_ACCEPT_FAIL,

    // ssl.c : ssl_cleanup()
    ERR_SSL_SHUTDOWN_FAIL,

    // requests.c: handle_client()
    ERR_CLIENT_READ_FAIL,
    ERR_FILE_READ_FAIL,

    // io.c: read_file()
    ERR_INVALID_SIZE,
    ERR_ALLOC_FAILED,
    ERR_READ_FAILED,
} FileError;

#endif
