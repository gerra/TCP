#ifndef EPOLLHANDLER_H
#define EPOLLHANDLER_H

#include <sys/epoll.h>
#include "tcpconnection.h"

class EpollHandler {
    int epollFD;
public:
    EpollHandler(int maxCount);
    ~EpollHandler();
    void addSocket(TCPSocket *newSocket, void *ptr, std::uint32_t events);
    void addSocket(TCPSocket *newSocket, int    fd, std::uint32_t events);
    void deleteSocket(TCPSocket *socket);
    int  getEvents(epoll_event *events, int maxActionsCount, int timeout = -1);
};

#endif // EPOLLHANDLER_H
