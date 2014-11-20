#include "server.h"
#define DEBUG

server::server() {
}


server::server(char * addr, char * port, int clientsCount) {
    tcpConnection.createAddress(addr, port);
    listener = tcpConnection.createBindingSocket();
    startListening(listener, clientsCount);
}

server::~server() {
    close(epollFD);
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        close(*it);
    }
}

void addEvent() {

}

void server::execute() {
    const int MAX_EVENTS = 10;
    epoll_event ev;
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
#ifdef DEBUG
    printf("changed %d fd\n", nfds);
#endif
    for (int i = 0; i < nfds; i++) {
        int curFD = events[i].data.fd;
#ifdef DEBUG
        printf("on sock %d event %d\n", curFD, events[i].events & EPOLLRDHUP);
#endif
        if (curFD == listener) {
            sockaddr_storage theirAddr;
            socklen_t addrLen;
            addrLen = sizeof(theirAddr);
            int newFD = accept(listener, (sockaddr *)&theirAddr, &addrLen);
            if (newFD == -1) {
                perror("accept");
            } else {
                /*std::pair<int, std::function<void()> > handler;
                handler.first = newFD;
                handler.second = std::function<void>([=]() {

                });*/

                ev.data.fd = newFD;
                ev.events = EPOLLIN | EPOLLRDHUP;

                epoll_ctl(epollFD, EPOLL_CTL_ADD, newFD, &ev);
                clients.insert(newFD);
                printf("server got connection from %s to socket %d\n",
                       getAddrAsString(theirAddr).c_str(), newFD);
#ifdef DEBUG
                printf("socket %d added\n", newFD);
#endif
/*???*/                setNonblocking(newFD);
            }
        } else {
            if (events[i].events & (EPOLLRDHUP | EPOLLHUP)) {
                printf("socket %d hung up!!!\n", curFD);
                close(curFD);
                clients.erase(curFD);
            } else if (events[i].events & EPOLLIN) {
                char buf[500];
                int nbytes = recieveFromFD(curFD, buf, 500);
#ifdef DEBUG
                printf("recieved %d bytes:\n", nbytes);
#endif
                if (nbytes == 0) {
                    close(curFD);
                    clients.erase(curFD);
                } else if (nbytes > 0) {
                    buf[nbytes] = '\0';
                    printf("%s", buf);
                    std::set<int> ex;
                    ex.insert(curFD);
                    sendToAllFromSet(clients, buf, nbytes, &ex);
                }
            }
        }
    }
}

void server::start() {
    printf("Starting server...\n");

    // using epoll

    epollFD = epoll_create(10);
    if (epollFD == -1) {
        perror("epoll_create");
        exit(1);
    }

#ifdef DEBUG
    printf("listener socket: %d\n", listener);
    printf("epoll socket: %d\n", epollFD);
#endif

    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listener;
    if ((epoll_ctl(epollFD, EPOLL_CTL_ADD, listener, &ev)) == -1) {
        perror("epoll_ctl");
        exit(2);
    }

    printf("Server ready to get data\n");
    while (running) {
        execute();
    }
}
