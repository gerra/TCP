#ifndef SERVER_H
#define SERVER_H

#include <set>
#include <functional>
#include <signal.h>
#include <memory>

#include <QtDebug>

#include "../tcpconnection.h"
#include "../epollhandler.h"

class server {  
    TCPConnection tcpConnection;
    /*
     * to do just an object and pass
     * it by reference everywhere
     */
    TCPSocket *listener; // listening socket
    EpollHandler *epoll; // epollHandler for managing of events
    bool running;
    std::set<TCPSocket*> clients; // file descriptors of connected clients

    void execute();
public:
    server(char *addr, char *port, int maxClientsCount);
    server(char *addr, char *port, int maxClientsCount, EpollHandler *epoll);
    ~server();
    void start();
    std::function<void(TCPSocket&)> onAccept;
};

#endif // SERVER_H
