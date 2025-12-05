#include <openssl/ssl.h>
#include <openssl/err.h>
#include "../include/errors.h"

SSL_CTX* ssl_initialize() {
    FileError err = ERR_NONE;
    const SSL_METHOD *method;
    SSL_CTX *ctx = NULL;

    // Initialize required resources from openSSL library
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // select TLS server method (TLS 1.2/1.3) and create a context using the method
    method = TLS_server_method();
    ctx = SSL_CTX_new(method);

    if (!ctx) {
        file_log(ERROR, "SSL context creation failed");
        err = ERR_SSL_CONTEXT_FAILED;
        goto handle_errors;
    }

    // disable insecure legacy protocols (SSLv2 and SSLv3)
    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);

    file_log(INFO, "SSL context initialized");
    return ctx;

handle_errors:
    switch (err) {
        case ERR_SSL_CONTEXT_FAILED: ERR_print_errors_fp(stderr);
        case ERR_NONE:
        default: return ctx;
    }
}

int ssl_load_certificates(SSL_CTX *ctx, const char *certificate, const char *privateKey) {
    FileError err = ERR_NONE;

    // Load server's public sertificate .pem file (sent to clients during TLS handshake)
    if (SSL_CTX_use_certificate_file(ctx, certificate, SSL_FILETYPE_PEM) <= 0) {
        file_log(ERROR, "failed to load certificate");
        err = ERR_CERT_FAIL;
        goto handle_errors;
    }

    // Load server's private key file
    if (SSL_CTX_use_PrivateKey_file(ctx, privateKey, SSL_FILETYPE_PEM) <= 0) {
        file_log(ERROR, "failed to load private key");
        err = ERR_PVTKEY_FAIL;
        goto handle_errors;
    }

    // verify that private key corresponds to the certificate
    if (!SSL_CTX_check_private_key(ctx)) {
        file_log(ERROR, "Private key does not match certificate");
        err = ERR_CERT_MISMATCH;
        goto handle_errors;
    }

    file_log(INFO, "SSL certificates loaded successfully");
    return 0;

handle_errors:
    switch (err) {
        case ERR_CERT_MISMATCH: 
        case ERR_PVTKEY_FAIL:
        case ERR_CERT_FAIL: ERR_print_errors_fp(stderr);
        case ERR_NONE:
        default: return -1;
    }

}

SSL* ssl_connect(SSL_CTX *ctx, int clientfd) {
    FileError err = ERR_NONE;
    SSL *ssl = NULL;

    // create new SSL structure for incoming client connection
    ssl = SSL_new(ctx);
    if (!ssl) {
        file_log(ERROR, "SSL object creation failed");
        err = ERR_SSL_FAIL;
        goto handle_errors;
    }

    // Bind the SSL object to clientfd (socket file descriptor)
    if (!SSL_set_fd(ssl, clientfd)) {
        file_log(ERROR, "Failed to associate SSL with client socket");
        err = ERR_SSL_SET_FD_FAIL;
        goto handle_errors;
    }

    // perform TLS handshake with the client
    if (SSL_accept(ssl) <= 0) {
        file_log(ERROR, "TLS handshake failed");
        err = ERR_SSL_ACCEPT_FAIL;
        goto handle_errors;
    }

    // TLS handshake successful
    return ssl;

handle_errors:
    switch (err) {
        case ERR_SSL_ACCEPT_FAIL:
        case ERR_SSL_SET_FD_FAIL: SSL_free(ssl);
        case ERR_SSL_FAIL: ERR_print_errors_fp(stderr);
        default: return NULL;
    }
}

void ssl_cleanup(SSL *ssl, SSL_CTX *ctx) {
    FileError err = ERR_NONE;

    if (ssl) {
        // 2 shutdowns required for a RFC-compliant bidirectional shutdown
        int shutdownStatus = SSL_shutdown(ssl);
        if (shutdownStatus == 1) {
            // SSL shutdown complete
            // file_log(INFO, "SSL shutdown complete");

        // if SSL_shutdown() returns 0 the shutdown is incomplete or waiting for peer
        } else if (shutdownStatus == 0) {
            file_log(WARN, "SSL shutdown incomplete");

        } else if (shutdownStatus < 0) {
            file_log(ERROR, "SSL shutdown error");
            err = ERR_SSL_SHUTDOWN_FAIL;
            goto handle_errors;
        }

        // free SSL session (objects)
        SSL_free(ssl);
    }

    if (ctx) {
        // free ssl context
        SSL_CTX_free(ctx);
    }

    // cleanup code for versions of OpenSSL < 1.1.0
    // EVP_cleanup(); 
    // CRYPTO_cleanup_all_ex_data();
    // ERR_free_strings();

    // OpenSSL resources cleaned up
    return;

handle_errors:
    switch (err) {
        case ERR_SSL_SHUTDOWN_FAIL: ERR_print_errors_fp(stderr);
        default: return;
    }
}
