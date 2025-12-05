#include <unistd.h>
#include <signal.h>
#include "types.h"
#include "errors.h"
#include "methods.h"

static ServerSocket server;
static SSLConfig sslConfig;
static ClientConnection client;

volatile sig_atomic_t serverActive = 1;

void signal_handler(int signal) {
    file_log(INFO, "");
    serverActive = 0;
}

void install_signals() {
    struct sigaction sa = {0};

    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

int server_initialize(int port) {
    FileError err = ERR_NONE;
    install_signals();

    //TODO add logging messages
    sslConfig.certificate = "certs/server.crt";
    sslConfig.privateKey = "certs/server.key";

    // socket initialization and configuration
    server.sockfd = socket_create();
    if (server.sockfd == -1) {
        err = ERR_SOCK_CREATION_FAIL;
        goto handle_errors;
    }
    socket_bind(server.sockfd, port);
    socket_listen(server.sockfd);

    // ssl context configuration
    sslConfig.ctx = ssl_initialize();
    if(!sslConfig.ctx) {
        err = ERR_SSL_CONTEXT_FAILED;
        goto handle_errors;
    }

    if (ssl_load_certificates(sslConfig.ctx, sslConfig.certificate, sslConfig.privateKey) < 0) {
        err = ERR_CERT_FAIL;
        goto handle_errors;
    }
    
    file_log(INFO, "Server initialized successfully");
    return 0;

handle_errors:
    switch (err) {
        case ERR_CERT_FAIL: ssl_cleanup(NULL, sslConfig.ctx);
        case ERR_SSL_CONTEXT_FAILED: close(server.sockfd);
        case ERR_SOCK_CREATION_FAIL: 
        default: return -1;
    }
}

void serve(int port) {
    char restart;

restart_server:
    if (server_initialize(port) < 0) {
        file_log(ERROR, "Server initialization failed");
        goto handle_errors;
    }

    while(serverActive) {
        client.clientfd = socket_connect(server.sockfd, &client.clientAddr);
        if (client.clientfd < 0) {
            if (errno == EINTR && !serverActive) break;
            continue;
        }

        client.ssl = ssl_connect(sslConfig.ctx, client.clientfd);
        if(!client.ssl) continue;

        handle_client(&client);
    }

    file_log(INFO, "Server shutdown complete");
    return;

handle_errors:
    puts("do you want to restart server [y/n]");
    scanf("%c", &restart);
    if (restart == 'y') {
        goto restart_server;

    } else if (restart == 'n') {
        ssl_cleanup(NULL, sslConfig.ctx);
        close(server.sockfd);
        file_log(INFO, "Server shutdown complete");

    } else {
        puts("Enter valid character");
        goto handle_errors;
    }
}
