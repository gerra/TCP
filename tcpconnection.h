#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <netdb.h>
#include <set>
#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

enum ERRORS {
    CONNECT_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    SOCKET_ERROR,
    GETADDR_ERROR,
    SETSOCK_ERROR,
    SELECT_ERROR,
    EPOLL_ERROR
};

/*
class TCPSocket {
    int fd;
public:
    TCPSocket() {

    }

    TCPSocket(string address, string port) {

    }

    TCPSocket(addrinfo *addr) {
        if (addr->ai_socktype != SOCK_STREAM) {
            throw "*addr socktype not SOCK_STREAM(may be you did use UDP?)";
        }
        if ((fd == socket(addr->ai_family,
                          addr->ai_socktype,
                          addr->ai_protocol)) == -1) {
            perror("socket");
            throw "unnable to create socket";
        }
    }

    int getFD() {
        return fd;
    }

    ~TCPSocket() {
        close(fd);
    }
};
*/

class TCPConnection {
    addrinfo hints;
    addrinfo *res;
    int sockfd;
public:

    TCPConnection();
    ~TCPConnection();
    void createAddress(char * address, char * port);
    // these 2 functions return socket file descriptor
    int createConnection();
    int createBindingSocket();
};

void sendToFD(int fd, char * msg, int msgSize);
void sendToAllFromFDSet(int fdMax, fd_set * fds,
                        char * msg, int msgSize,
                        std::set<int> * exception);
void sendToAllFromSet(std::set<int> const& st,
                      char *msg, int msgSize,
                      std::set<int> * exception);
void startListening(int fd, int count);
std::string getAddrAsString(sockaddr_storage & addr);

/*
 * returns:
 *  < 0 if error
 *  = 0 if fd was closed
 *  > 0 is msgSize
 */
int recieveFromFD(int fd, char * buf, int maxSize);
int setNonblocking(int fd);

#endif // TCPCONNECTION_H
