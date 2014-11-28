#ifndef EPOLLHANDLER_H
#define EPOLLHANDLER_H

#include <sys/epoll.h>
#include "tcpconnection.h"

class EpollException : std::exception {
    std::string msg;
public:
    EpollException(std::string msg) : msg(msg) {}
    EpollException(const char *msg) : msg(msg) {}
    std::string getMessage() {
        return msg;
    }
};

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
