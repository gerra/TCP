#include "server.h"
#include <iostream>

server::server(char * addr, char * port, int clientsCount) {
    tcpConnection.createAddress(addr, port);
    listener = tcpConnection.createBindingSocket();
    setNonblocking(listener);
    startListening(listener, clientsCount);
}

server::~server() {
    close(epollFD);
    close(listener);
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        close(*it);
    }
}

void server::execute() {
    const int MAX_EVENTS = 10;
    epoll_event ev;
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
    qDebug() << "changed " << nfds << " sockets:";
    for (int i = 0; i < nfds; i++) {
        typedef std::pair<int, std::function<void(char *, int)> > PIF;
        PIF * curEventHandler;
        curEventHandler = static_cast<PIF*>(events[i].data.ptr);
        int curFD = curEventHandler->first;
        qDebug() << "\ton sock " << curFD << " happened event " << events[i].events << ":";
        if (curFD == listener) {
            sockaddr_storage theirAddr;
            socklen_t addrLen;
            addrLen = sizeof(theirAddr);
            int newFD = accept(listener, (sockaddr *)&theirAddr, &addrLen);
            if (newFD == -1) {
                perror("accept");
            } else {
                setNonblocking(newFD);
                PIF * handler = new PIF();
                handler->first = newFD;
                handler->second = std::function<void(char *, int)>([=](char *msg, int msgSize) {
                    //std::cout << "i'm " << " " << newFD << "\n";
                    sendToFD(newFD, msg, msgSize);
                });

                ev.data.ptr = handler;
                ev.events = EPOLLIN | EPOLLRDHUP;

                epoll_ctl(epollFD, EPOLL_CTL_ADD, newFD, &ev);
                clients.insert(newFD);
                printf("\t\tserver got connection from %s to new socket %d\n",
                       getAddrAsString(theirAddr).c_str(), newFD);
            }
        } else {
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                printf("socket %d hung up!!!\n", curFD);
                close(curFD);
                clients.erase(curFD);
                delete curEventHandler;
            } else if (events[i].events & EPOLLIN) {
                char buf[500];
                int nbytes = recieveFromFD(curFD, buf, 500);
                qDebug() << "\t\trecieved " << nbytes << " bytes:";
                if (nbytes == 0) {
                    close(curFD);
                    clients.erase(curFD);
                    delete curEventHandler;
                } else if (nbytes > 0) {
                    buf[nbytes] = '\0';
                    printf("%s", buf);
                    std::set<int> ex;
                    ex.insert(curFD);
                    sendToAllFromSet(clients, buf, nbytes, &ex);
                    //curEventHandler->second(buf, nbytes);
                    //std::cout << &curEventHandler->second << "\n";
                }

            }
        }
    }
}

void server::start() {
    printf("Starting server...\n");
    epollFD = epoll_create(10);
    if (epollFD == -1) {
        perror("epoll_create");
        exit(EPOLL_ERROR);
    }
    qDebug() << "listener socket: " << listener;
    qDebug() << "epoll socket: " << epollFD;

    std::pair<int, std::function<void()> > handler;
    handler.first = listener;
    handler.second = std::function<void()>([=]() {
        std::cout << listener << "\n";
    });
    epoll_event ev;
    ev.data.ptr = &handler;
    ev.events = EPOLLIN;

    if ((epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &ev)) == -1) {
        perror("epoll_ctl");
        exit(EPOLL_ERROR);
    }

    printf("Server ready to get data\n");
    while (running) {
        execute();
    }
}
