#ifndef SERVER_H
#define SERVER_H

#include "../tcpconnection.h"
#include <sys/epoll.h>
#include <functional>

class server {  
    TCPConnection tcpConnection;
    int listener; // listening socket
    int epollFD; // epoll socket
    bool running;
    std::set<int> clients; // file descriptors of connected clients

    void execute();
public:
    server();
    ~server();
    server(char * addr, char * port, int clientsCount);
    void start();
};

#endif // SERVER_H
