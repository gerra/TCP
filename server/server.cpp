#include "server.h"
#include <iostream>

#include <stdint.h>

typedef std::function<void(std::uint32_t)> FVUI32;
typedef std::pair<TCPSocket *, FVUI32 > PSF;

server::server(char * addr, char * port, int clientsCount) {
    listener = NULL;
    epoll = NULL;
    try {
        tcpConnection.createAddress(addr, port);
        listener = tcpConnection.createBindingSocket();
        listener->setNonBlocking();
        listener->startListening(clientsCount);
    } catch (TCPException &e) {
        if (listener != NULL) {
            delete listener;
        }
        throw;
    }
}

server::~server() {
    qDebug() << "deleting server starts...";
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if (epoll != NULL) {
            epoll->deleteSocket(*it);
        }
        delete *it;
    }

    if (listener != NULL) {
        if (epoll != NULL) {
            epoll->deleteSocket(listener);
            delete epoll;
        }
        delete listener;
    } else {
        if (epoll != NULL) {
            delete epoll;
        }
    }
    clients.clear();
    qDebug() << "...deleting server ends";
}

void server::execute() {
    const int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];
    int nfds = epoll->getEvents(events, MAX_EVENTS);
    if (nfds < 0) {
        return;
    }
    qDebug() << "changed " << nfds << " sockets:";
    for (int i = 0; i < nfds; i++) {
        PSF *curEventHandler = static_cast<PSF *>(events[i].data.ptr);
        TCPSocket *curSocket = curEventHandler->first;
        if (curSocket == listener || clients.find(curSocket) != clients.end()) {
            curEventHandler->second(events[i].events);
        }
    }
}

volatile sig_atomic_t flag = 0;
void my_function(int) { // can be called asynchronously
  flag = 1; // set flag
}

void server::start() {
    running = true;
    printf("Starting server...\n");
    epoll = new EpollHandler(10);
    qDebug() << "listener socket: " << listener;
    qDebug() << "epoll socket: " << epoll;

    PSF *handler = new PSF();
    handler->first = listener;
    /*
     * This function will be called when somebody 'knocks' to our server
     */
    handler->second = FVUI32([=](std::uint32_t) {
        qDebug() << "I'm listener of new connections";
        sockaddr_storage theirAddr;
        socklen_t addrLen;
        addrLen = sizeof(theirAddr);
        TCPSocket *newSocket = listener->acceptToNewSocket((sockaddr *)&theirAddr, &addrLen);
        if (newSocket != NULL) {
            newSocket->setNonBlocking();
            PSF *clientEvent = new PSF();
            clientEvent->first = newSocket;
            /*
             * This function will be called when socket, who recieves data, changes
             */
            clientEvent->second = FVUI32([=](std::uint32_t events) {
                qDebug() << "I'm listener of client's from socket " << newSocket->getFD() << " changes";
                if (events & (EPOLLRDHUP | EPOLLHUP)) {
                    printf("socket %d hung up!!!\n", newSocket->getFD());
                    clients.erase(newSocket);
                    epoll->deleteSocket(newSocket);
                    delete newSocket;
                    qDebug() << "Deleting event...";
                    delete clientEvent;
                } else if (events & EPOLLIN) {
                    try {
                        onAccept(*newSocket);
                    } catch (...) {
                        clients.erase(newSocket);
                        epoll->deleteSocket(newSocket);
                        delete newSocket;
                        delete clientEvent;
                        throw;
                    }

                    /*
                     * if user closed socket in his function
                     */
                    if (newSocket->isClosed()) {
                        clients.erase(newSocket);
/*???*/                        //epoll->deleteSocket(newSocket); // it is managing by EPOLLHUP
                        delete newSocket;
                        delete clientEvent;
                    }
                }
            });
            clients.insert(newSocket);
            try {
                epoll->addSocket(newSocket, static_cast<void*>(clientEvent), EPOLLIN | EPOLLRDHUP);
                printf("\t\tserver got connection from %s to new socket %d\n",
                       getAddrAsString(theirAddr).c_str(), newSocket->getFD());
            } catch (EpollException &e) {
                std::cerr << "Unnable to add client: " + e.getMessage() << "\n";
                clients.erase(newSocket);
                delete newSocket;
                delete clientEvent;
            }
        }
    });

    try {
        epoll->addSocket(listener, static_cast<void *>(handler), EPOLLIN);
    } catch (EpollException &e) {
        delete handler;
        throw TCPException("Unnable to start server: " + e.getMessage());
    }

    printf("Server ready to get data\n");

    signal(SIGINT, my_function);
    while (running) {
        execute();
        if (flag) {
            std::cout << "\n";
            running = false;
        }
    }
}
