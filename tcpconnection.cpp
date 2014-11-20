#include "tcpconnection.h"

#include <string.h>

TCPConnection::TCPConnection() {
    connectionWasCreated = false;
    bindingWasCreated = false;
    sockfd = -1;
}

void TCPConnection::createAddress(char *address, char *port) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // whatever ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // tcp connection
    int rv;
    if ((rv = getaddrinfo(address, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(GETADDR_ERROR);
    }
}

int TCPConnection::createConnection() {
    addrinfo * stableAddr = NULL;
    int resSock = -1;
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
        resSock = sockfd;
        break; // we found good address
    }
    if (stableAddr == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(CONNECT_ERROR);
    }
    connectionWasCreated = true;
    return resSock;
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
                        int msgSize, std::set<int> * exception) {
    for (int i = 0; i <= fdMax; i++) {
        if (FD_ISSET(i, fds) &&
                ((exception && exception->find(i) == exception->end()) ||
                        !exception)) {
            sendToFD(i, msg, msgSize);
        }
    }
}

void sendToAllFromSet(std::set<int> const& st, char *msg,
                      int msgSize, std::set<int> * exception) {
    std::set<int>::iterator it = st.begin();
    for (; it != st.end(); ++it) {
        if (!exception ||
                (exception && exception->find(*it) == exception->end())) {
            sendToFD(*it, msg, msgSize);
        }
    }
}

int recieveFromFD(int fd, char * buf, int maxSize) {
    int nbytes;
    if ((nbytes = recv(fd, buf, maxSize, 0)) <= 0) {
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

void startListening(int fd, int count) {
    if (listen(fd, count) == -1) {
        perror("listen");
        exit(LISTEN_ERROR);
    }
}

void *getInAddr(sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((sockaddr_in*)sa)->sin_addr);
    }
    return &(((sockaddr_in6*)sa)->sin6_addr);
}

std::string getAddrAsString(sockaddr_storage &addr) {
    char s[INET6_ADDRSTRLEN];
    inet_ntop(addr.ss_family, getInAddr((sockaddr*)&addr), s, sizeof(s));
    std::string ss(s);
    return ss;
}

#include <sys/ioctl.h>
int setNonblocking(int fd) {
    int flags;

    /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined O_NONBLOCK
    /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
    if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
        flags = 0;
    }
    return {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }
#else
    /* Otherwise, use the old way of doing it */
    flags = 1;
    return ioctl(fd, FIONBIO, &flags);
#endif
}
