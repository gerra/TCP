#ifndef CLIENT_H
#define CLIENT_H

#include "../tcpconnection.h"
#include <sys/epoll.h>
#include <QtDebug>

class client
{
    TCPConnection tcpConnection;
    int talker; // client socket
    int epollFD; // epoll socket
    bool running;

    /*
     * returns:
     *  < 0 - server stopped
     *  > 0 - connection is ok
     */
    int execute();
public:
    client();
    void connectTo(char *addr, char *port);
    void start();
    ~client();
};

#endif // CLIENT_H
