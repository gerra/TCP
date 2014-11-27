#include "tcpconnection.h"

#include <string.h>

TCPConnection::TCPConnection() {
}

void TCPConnection::createAddress(char *address, char *port) {
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // whatever ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM; // tcp connection
    int rv;
    if ((rv = getaddrinfo(address, port, &hints, &res)) != 0) {
        throw TCPException("getaddrinfo: " + std::string(gai_strerror(rv)));
    }
}

TCPSocket *TCPConnection::createConnection() {
    addrinfo * stableAddr = NULL;
    int resSock = -1;
    int sockfd;
    for (stableAddr = res; stableAddr != NULL; stableAddr = stableAddr->ai_next) {
        if (stableAddr->ai_socktype != SOCK_STREAM) {
            continue;
        }
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
        throw TCPException("Failed to connect");
    }
    return new TCPSocket(resSock);
}

TCPSocket *TCPConnection::createBindingSocket() {
    addrinfo * stableAddr = NULL;
    int resSock = -1;
    int sockfd;
    for (stableAddr = res; stableAddr != NULL; stableAddr = stableAddr->ai_next) {
        if (stableAddr->ai_socktype != SOCK_STREAM) {
            continue;
        }
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
            throw TCPException("Failed to set socket");
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
        throw TCPException("Failed to bind");
    }
    return new TCPSocket(resSock);
}

TCPConnection::~TCPConnection() {
    freeaddrinfo(res);
}

/*void sendToAllFromSet(std::set<int> const& st, char *msg,
                      int msgSize, std::set<int> * exception) {
    std::set<int>::iterator it = st.begin();
    for (; it != st.end(); ++it) {
        if (!exception ||
                (exception && exception->find(*it) == exception->end())) {
            sendToFD(*it, msg, msgSize);
        }
    }
}*/

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
