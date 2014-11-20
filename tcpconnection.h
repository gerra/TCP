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
    SELECT_ERROR
};

class TCPConnection {
    addrinfo hints;
    addrinfo *res;
    int sockfd;
    bool connectionWasCreated;
    bool bindingWasCreated;
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
