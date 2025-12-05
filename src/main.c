#include <stdlib.h>
#define DEFAULT_HTTPS_PORT 8443

void serve(int port);

int main(int argc, char* argv[]) {
    // Verify port number validity if provided
    int port = (argc >= 2)? atoi(argv[1]) : DEFAULT_HTTPS_PORT;
    if (port <= 0 || port >= 65535) port = DEFAULT_HTTPS_PORT;

    serve(port);
    return 0;
}
