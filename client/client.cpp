#include "client.h"

client::client() {

}

client::~client() {
    epoll_ctl(epollFD, EPOLL_CTL_DEL, talker, NULL);
    close(epollFD);
    close(talker);
}

int client::execute() {
    const int MAX_EVENTS = 10;
    epoll_event events[MAX_EVENTS];
    int nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
    for (int i = 0; i < nfds; i++) {
        int curFD = events[i].data.fd;
        if (events[i].events & EPOLLRDHUP) {
            return -1;
        } else if (events[i].events & EPOLLIN) {
            char buf[500];
            int nbytes = recieveFromFD(curFD, buf, 500);
            if (nbytes == 0) {
                return -1;
            } else if (nbytes > 0) {
                buf[nbytes] = '\0';
                printf("%s", buf);
            }
            // < 0 - error with recieve
        }
    }
    return 1;
}

void client::connectTo(char *addr, char *port) {
    tcpConnection.createAddress(addr, port);
    talker = tcpConnection.createConnection();
    setNonblocking(talker);
}

void client::start() {
    running = true;
    epollFD = epoll_create(10);
    if (epollFD == -1) {
        perror("epoll_create");
        //exit(EPOLL_ERROR);
        throw EPOLL_ERROR;
    }
    epoll_event ev;
    ev.data.fd = talker;
    ev.events = EPOLLIN | EPOLLRDHUP;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, talker, &ev);

    char * msg = "Hello, world!!!\n\0";
    sendToFD(talker, msg, strlen(msg));

    while (running) {
        int resultCode = execute();
        if (resultCode < 0) {
            printf("Connection closed by foreign host\n");
            running = false;
            close(talker);
        }
    }
}
