#include "../include/server.h"
#include "../include/requests.h"
#include "../include/errors.h"

void handle_client(ClientConnection *conn) {
    FileError err = ERR_NONE;
    char buffer[BUFFER_SIZE] = {0};
    int bytes;

    // read http request from client over SSL
    bytes = SSL_read(conn->ssl, buffer, BUFFER_SIZE-1);
    if (bytes <= 0) {
        file_log(ERROR, "failed to read from client");
        err = ERR_CLIENT_READ_FAIL;
        goto handle_errors;
    }
    
    file_log(DEBUG, "Recieved request: %.*s", (int)strcspn(buffer, "\r\n"), buffer);

    // Route: GET /
    // character after / must be a space to match the route
    if (strncmp(buffer, "GET / ", 6) == 0) {
        // files are saved to dynamically allocated memory by file_read()
        char *body = read_file("./html/index.html");
        if (body == NULL) {
            err = ERR_FILE_READ_FAIL;
            goto handle_errors;
        }

        respond(conn->ssl, "200 OK", "text/html", body);
        free(body);

    // Route: GET /health
    } else if (strncmp(buffer, "GET /health", 11) == 0) {
        char *body = "{\"status\":\"healthy\"}\n";
        respond(conn->ssl, "200 OK", "application/json", body);

    // Route: 404 not found
    } else {
        char *body = read_file("./html/404.html");
        if (body == NULL) {
            err = ERR_FILE_READ_FAIL;
            goto handle_errors;
        }

        respond(conn->ssl, "404 Not Found", "text/html", body);
        free(body);
    }

handle_errors:
    switch (err) {
        case ERR_NONE:
        case ERR_FILE_READ_FAIL:
        case ERR_CLIENT_READ_FAIL:
        default:
            SSL_shutdown(conn->ssl);
            ssl_cleanup(conn->ssl, NULL);
            close(conn->clientfd);
            return;
    }
}

void respond(SSL *ssl, const char *status, const char *contentType, char *body) {
    char response[RESPONSE_BUFFER_SIZE];
    int bodyLen = strlen(body);

    // https response string
    snprintf(response, sizeof(response),
            "HTTP/1.1 %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            status, contentType, bodyLen, body);

    if (SSL_write(ssl, response, strlen(response)) <= 0) {
        file_log(ERROR, "SSL write failed");
        return;
    }

    file_log(DEBUG, "Response sent: \e[0;35m%s\e[0m", status);
    return;
    // no further error handling
}
