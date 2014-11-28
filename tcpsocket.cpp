#include "tcpsocket.h"

TCPSocket::TCPSocket(int fd) {
    sockfd = fd;
    wasClosed = false;
}

TCPSocket::TCPSocket(addrinfo *addr) {
    if (addr->ai_socktype != SOCK_STREAM) {
        throw TCPException("Bad socket type");
    }
    wasClosed = false;
    sockfd = socket(addr->ai_family,
                    addr->ai_socktype,
                    addr->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        throw TCPException("Failed to create socket");
    }
}

TCPSocket::~TCPSocket() {
    if (sockfd >= 0 && !wasClosed) {
        close(sockfd);
    }
}

void TCPSocket::closeSocket() {
    if (sockfd >= 0 && !wasClosed) {
        close(sockfd);
        wasClosed = true;
    }
}

void TCPSocket::reusePort() {
    // reusing the port
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &yes, sizeof(int)) == -1) {
        perror("setsockport");
        throw TCPException("Failed to set socket on port");
    }
}

void TCPSocket::bindSocket(addrinfo *addr) {
    int res = bind(sockfd, addr->ai_addr, addr->ai_addrlen);
    if (res == -1) {
        perror("bind");
        throw TCPException("Failed to bind");
    }
}

void TCPSocket::connectToAddr(addrinfo *addr) {
    int res = connect(sockfd, addr->ai_addr, addr->ai_addrlen);
    if (res == -1) {
        perror("connect: ");
        throw TCPException("Failed to connect");
    }
}

bool TCPSocket::isClosed() {
    return wasClosed;
}

int TCPSocket::getFD() {
    return sockfd;
}

int TCPSocket::setNonBlocking() {
    int flags;
    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined O_NONBLOCK
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(sockfd, F_GETFL, 0))) {
        flags = 0;
    }
    return {
        fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    }
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(sockfd, FIONBIO, &flags);
#endif
}

void TCPSocket::sendMsg(char *msg) {
    int msgSize = strlen(msg);
    if (send(sockfd, msg, msgSize, 0) == -1) {
        perror("send");
    }
}

int TCPSocket::recieveMsg(char * buf, int maxSize) {
    int nbytes;
    if ((nbytes = recv(sockfd, buf, maxSize, 0)) <= 0) {
        if (nbytes == 0) {
            fprintf(stdout, "socket %d hung up\n", sockfd);
            return 0;
        } else {
            perror("recv");
            throw TCPException("Recieve error");
        }
    }
    buf[nbytes] = '\0';
    return nbytes;
}

void TCPSocket::startListening(int count) {
    if (listen(sockfd, count) == -1) {
        perror("listen");
        throw TCPException("Listen error");
    }
}

TCPSocket* TCPSocket::acceptToNewSocket(sockaddr *addr, socklen_t *len) {
    int newFD = accept(sockfd, addr, len);
    if (newFD == -1) {
        perror("accept");
        return NULL;
    } else {
        return new TCPSocket(newFD);
    }
}
