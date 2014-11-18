#ifndef SERVER_H
#define SERVER_H

#include "../tcpconnection.h"

class server {
    enum ERRORS {
        SELECT_ERROR
    };
    TCPConnection tcpConnection;
    int listener; // listening socket
public:
    server();
    server(char * addr, char * port, int clientsCount);
    void start();
};

#endif // SERVER_H
