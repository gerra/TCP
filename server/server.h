#ifndef SERVER_H
#define SERVER_H

#include "../tcpconnection.h"
#include <sys/epoll.h>
#include <functional>
#include <QtDebug>

class server {  
    TCPConnection tcpConnection;
    int listener; // listening socket
    int epollFD; // epoll socket
    bool running;
    std::set<int> clients; // file descriptors of connected clients

    void execute();

    std::function<void()> doAfterAccept;
public:
    server(char *addr, char *port, int clientsCount);
    ~server();
    void start();
};

#endif // SERVER_H
