#include "server.h"
#include <iostream>

#include <stdint.h>

typedef std::function<void(std::uint32_t)> FVUI32;
typedef std::pair<int, FVUI32 > PIF;

server::server(char * addr, char * port, int clientsCount) {
    tcpConnection.createAddress(addr, port);
    listener = tcpConnection.createBindingSocket();
    setNonblocking(listener);
    startListening(listener, clientsCount);
}

server::~server() {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        epoll_ctl(epollFD, EPOLL_CTL_DEL, *it, NULL);
        close(*it);
    }
    close(epollFD);
    close(listener);
    //this = NULL;
}

void server::execute() {
    const int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
    qDebug() << "changed " << nfds << " sockets:";
    for (int i = 0; i < nfds; i++) {
        PIF *curEventHandler = static_cast<PIF *>(events[i].data.ptr);
        curEventHandler->second(events[i].events);
        /*typedef std::pair<int, std::function<void(char *, int)> > PIF;
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
        }*/
    }
}

void server::start() {
    printf("Starting server...\n");
    epollFD = epoll_create(10);
    if (epollFD == -1) {
        perror("epoll_create");
        //exit(EPOLL_ERROR);
        throw EPOLL_ERROR;
    }
    qDebug() << "listener socket: " << listener;
    qDebug() << "epoll socket: " << epollFD;

    PIF *handler = new PIF();
    handler->first = listener;
    /*
     * This function will be called when somebody 'knocks' to our server
     */
    handler->second = FVUI32([=](std::uint32_t) {
        qDebug() << "I'm listener of new connections\n";
        sockaddr_storage theirAddr;
        socklen_t addrLen;
        addrLen = sizeof(theirAddr);
        int newFD = accept(listener, (sockaddr *)&theirAddr, &addrLen);
        if (newFD == -1) {
            perror("accept");
        } else {
            setNonblocking(newFD);
            PIF *clientEvent = new PIF();
            clientEvent->first = newFD;
            /*
             * This function will be called when socket, who recieves data, changes
             */
            clientEvent->second = FVUI32([=](std::uint32_t events) {
                qDebug() << "I'm listener of client's from socket " << newFD << " changes";
                if (events & (EPOLLRDHUP | EPOLLHUP)) {
                    printf("socket %d hung up!!!\n", newFD);
                    close(newFD);
                    clients.erase(newFD);
                    //delete curEventHandler;
                    epoll_ctl(epollFD, EPOLL_CTL_DEL, newFD, NULL);
                } else if (events & EPOLLIN) {
                    char buf[500];
                    int nbytes = recieveFromFD(newFD, buf, 500);
                    qDebug() << "\t\trecieved " << nbytes << " bytes:";
                    if (nbytes == 0) {
                        close(newFD);
                        clients.erase(newFD);
                        //delete curEventHandler;
                        epoll_ctl(epollFD, EPOLL_CTL_DEL, newFD, NULL);
                    } else if (nbytes > 0) {
                        // TODO handler for onRecieve
                        buf[nbytes] = '\0';
                        printf("%s", buf);
                        //std::set<int> ex;
                        //ex.insert(newFD);
                        sendToAllFromSet(clients, buf, nbytes, NULL);
                    }
                }
            });
            epoll_event e;
            e.events = EPOLLIN | EPOLLRDHUP;
            e.data.ptr = static_cast<void *>(clientEvent);
            clients.insert(newFD);
            epoll_ctl(epollFD, EPOLL_CTL_ADD, newFD, &e);
            printf("\t\tserver got connection from %s to new socket %d\n",
                   getAddrAsString(theirAddr).c_str(), newFD);
        }
    });
    epoll_event ev;
    ev.data.fd = listener;
    ev.data.ptr = static_cast<void *>(handler);
    ev.events = EPOLLIN;

    if ((epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &ev)) == -1) {
        perror("epoll_ctl");
        //exit(EPOLL_ERROR);
        throw EPOLL_ERROR;
    }

    printf("Server ready to get data\n");
    while (running) {
        execute();
    }
}
