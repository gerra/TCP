#include "tcpsocket.h"

TCPSocket::TCPSocket(int fd) {
    sockfd = fd;
}

TCPSocket::~TCPSocket() {
    if (sockfd >= 0) {
        close(sockfd);
    }
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
            //throw RECIEVE_ERROR;
            throw TCPException("Recieve error");
        }
    }
    buf[nbytes] = '\0';
    return nbytes;
}

void TCPSocket::startListening(int count) {
    if (listen(sockfd, count) == -1) {
        perror("listen");
        //exit(LISTEN_ERROR);
        //throw LISTEN_ERROR;
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
