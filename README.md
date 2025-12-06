# HTTP/HTTPS Server

A lightweight HTTP/HTTPS server written in pure C, using only low level system libraries and OpenSSL.

## Features
### Low level socket handling
* uses basic POSIX networking libraries
* fully manual control over socket creation, binding, listening, and accepting client connectons
* shutdown handling with `SIGINT`/`SIGTERM`
### SSL/TLS support (openSSL)
* Automatic certificate and key file loading and authentication
* SSL Initialization, context management, encrypted read/write operations
* Works with TLS 1.2+
### Static file serving and response handling
* Supports standard HTTP GET requests
* Serves static HTML assets
### Detailed logging
* Error logging and response logging
* Time included
### Libraries used:
* `stdio.h`, `stdlib.h`, `time.h`,  `unistd.h`, `signal.h`
* `sys/socket.h`
* `netinet/in.h`
* `arpa/inet.h`
* `openssl/ssl.h`
* `openssl/err.h`
## Structure

The file structure below is required to build and execute the program, including valid SSL certificates and private keys.
```
.
├── build
│   ├── c-http-server
│   ├── io.o
│   ├── main.o
│   ├── requests.o
│   ├── server.o
│   ├── socket.o
│   └── ssl.o
├── certs
│   ├── server.crt
│   └── server.key
├── html
│   ├── 404.html
│   └── index.html
├── include
│   ├── errors.h
│   ├── requests.h
│   └── server.h
├── logs
│   └── server.log
├── Makefile
├── README.md
└── src
    ├── io.c
    ├── main.c
    ├── requests.c
    ├── server.c
    ├── socket.c
    └── ssl.c

7 directories, 23 files
```
## Setup and Initialization
#### 1. Install dependencies
For Linux (Debian based)
```bash
sudo apt update
sudo apt install build-essential libssl-dev
```

#### 2. Clone the repository
```bash
git clone https://https://github.com/C-HTTP-server.git
cd C-HTTP-server
```

#### 3. Build the server
```bash
make
```

#### 4. Run the server

If TLS certificates or private keys are not present, generate them in the `certs/` directory.
```bash
cd certs/
openssl req -x509 -newkey rsa:2048 -keyout server.key -out server.crt -days 3650 -nodes
```

Run the server:
```bash
cd ..
make run
```

Local testing (optional):
```bash
curl -k http://localhost:8443
```
for port forwarding, the port 8443 has to be opened using ufw or a similar method.