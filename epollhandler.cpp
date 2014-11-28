#include "epollhandler.h"

EpollHandler::EpollHandler(int maxCount) {
    epollFD = epoll_create(maxCount);
    if (epollFD == -1) {
        perror("epoll_create");
        throw TCPException("Creating epoll error");
    }
}

EpollHandler::~EpollHandler() {
    if (epollFD >= 0) {
        close(epollFD);
    }
}

void EpollHandler::addSocket(TCPSocket *newSocket, int fd, std::uint32_t events) {
    if (newSocket != NULL) {
        epoll_event ev;
        ev.data.fd = fd;
        ev.events = events;
        if ((epoll_ctl(epollFD, EPOLL_CTL_ADD, newSocket->getFD(), &ev)) == -1) {
            perror("epoll_ctl");
            throw TCPException("Adding socket to epoll error");
        }
    }
}

void EpollHandler::addSocket(TCPSocket *newSocket, void *ptr, std::uint32_t events) {
    if (newSocket != NULL) {
        epoll_event ev;
        ev.data.ptr = ptr;
        ev.events = events;
        if ((epoll_ctl(epollFD, EPOLL_CTL_ADD, newSocket->getFD(), &ev)) == -1) {
            perror("epoll_ctl");
            throw TCPException("Adding socket to epoll error");
        }
    }
}

void EpollHandler::deleteSocket(TCPSocket *socket) {
    if (socket != NULL) {
        if ((epoll_ctl(epollFD, EPOLL_CTL_DEL, socket->getFD(), NULL)) == -1) {
            perror("epoll_ctl");
            //throw TCPException("Deleting socket from epoll error");
        }
    }
}

int EpollHandler::getEvents(epoll_event *events, int maxEventsCount, int timeout) {
    return epoll_wait(epollFD, events, maxEventsCount, timeout);
}
