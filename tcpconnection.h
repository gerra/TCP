#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include <netdb.h>
#include <set>
#include <cstdlib>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>

class TCPConnection {

    enum ERRORS {
        BIND_ERROR,
        LISTEN_ERROR,
        SOCKET_ERROR,
        GETADDR_ERROR,
        SETSOCK_ERROR
    };

    addrinfo hints;
    addrinfo *res;
    int sockfd;
    bool connectionWasCreated;
    bool bindingWasCreated;
public:
    TCPConnection();
    ~TCPConnection();
    void createAddress(char * address, char * port);
    void createConnection();
    int createBindingSocket();
    void startListening(int count);

    int getSocket();
};

void sendToFD(int fd, char * msg, int msgSize);
void sendToAllFromFDSet(int fdMax, fd_set * fds,
                        char * msg, int msgSize,
                        std::set<int> * except);

/*
 * returns:
 *  < 0 if error
 *  = 0 if fd was closed
 *  > 0 is msgSize
*/
int recieveFromFD(int fd, char * buf);

#endif // TCPCONNECTION_H
