#include "tcpconnection.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

TCPConnection::TCPConnection() {
    connectionWasCreated = false;
    bindingWasCreated = false;
    sockfd = -1;
}

void TCPConnection::createAddress(char *address, char *port) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // whatever ipv4 pr ipv6
    hints.ai_socktype = SOCK_STREAM; // tcp connection
    int rv;
    if ((rv = getaddrinfo(address, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(GETADDR_ERROR);
    }
}

void TCPConnection::createConnection() {
    addrinfo * stableAddr = NULL;
    for (stableAddr = res; stableAddr != NULL; stableAddr = stableAddr->ai_next) {
        if ((sockfd = socket(stableAddr->ai_family,
                             stableAddr->ai_socktype,
                             stableAddr->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        if (connect(sockfd, stableAddr->ai_addr, stableAddr->ai_addrlen) == -1) {
            close(sockfd);
            perror("connect: ");
            continue;
        }
        break; // we found good address
    }
    if (stableAddr == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(2);
    }
    connectionWasCreated = true;
}

int TCPConnection::createBindingSocket() {
    addrinfo * stableAddr = NULL;
    int resSock = -1;
    for (stableAddr = res; stableAddr != NULL; stableAddr = stableAddr->ai_next) {
        if ((sockfd = socket(stableAddr->ai_family,
                             stableAddr->ai_socktype,
                             stableAddr->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }
        // reusing the port
        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &yes, sizeof(int)) == -1) {
            perror("setsockport");
            exit(SETSOCK_ERROR);
        }
        if (bind(sockfd, stableAddr->ai_addr,
                 stableAddr->ai_addrlen) == -1) {
            close(sockfd);
            perror("bind");
            continue;
        }
        resSock = sockfd; // we found good address
        break;
    }
    if (stableAddr == NULL) {
        fprintf(stderr, "failed to bind\n");
        exit(2);
    }
    bindingWasCreated = true;
    return resSock;
}

void TCPConnection::startListening(int count) {
    if (listen(sockfd, count) == -1) {
        perror("listen");
        exit(LISTEN_ERROR);
    }
}

int TCPConnection::getSocket() {
    return sockfd;
}

TCPConnection::~TCPConnection() {
    freeaddrinfo(res);
    if (sockfd != -1) {
        close(sockfd);
    }
}

void sendToFD(int fd, char *msg, int msgSize) {
    if (send(fd, msg, msgSize, 0) == -1) {
        perror("send");
    }
}

void sendToAllFromFDSet(int fdMax, fd_set *fds, char *msg,
                        int msgSize, std::set<int> * except = NULL) {
    for (int i = 0; i <= fdMax; i++) {
        if (FD_ISSET(i, fds) &&
                ((except && except->find(i) == except->end()) || !except)) {
            sendToFD(i, msg, msgSize);
        }
    }
}

int recieveFromFD(int fd, char * buf) {
    int nbytes;
    if ((nbytes = recv(fd, buf, sizeof(*buf), 0)) <= 0) {
        if (nbytes == 0) {
            fprintf(stdout, "socket %d hung up\n", fd);
            return 0;
        } else {
            perror("recv");
            return -1;
        }
    }
    return nbytes;
}
