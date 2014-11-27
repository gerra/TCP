#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <netdb.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>

#include "tcpexception.h"

class TCPSocket {
    int sockfd;
public:
    TCPSocket(int fd);
    ~TCPSocket();
    int getFD();
    int setNonBlocking();
    void sendMsg(char *msg);
    int recieveMsg(char * buf, int maxSize);
    void startListening(int count);
    TCPSocket *acceptToNewSocket(sockaddr *addr, socklen_t *len);
};

#endif // TCPSOCKET_H
