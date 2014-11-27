#include <iostream>
#include <sys/epoll.h>

#include <cstring>

#include "client.h"

int main() {
    client myClient;
    try {
        myClient.connectTo("127.0.0.1", "2323");
        myClient.start();
    } catch (TCPException& e) {
        std::cout << e.getMessage() << "\n";
    }

//    return 0;
    /*
    //char addr[100], port[10];
    //std::cin >> addr >> port;
    char * addr, * port;
    addr = "127.0.0.1"; port = "2323";
    TCPConnection tcpConnection;
    tcpConnection.createAddress(addr, port);
    int sockfd = tcpConnection.createConnection();

    int epollFD = epoll_create(10);
    if (epollFD == -1) {
        perror("epoll_create");
        exit(1);
    }

    epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = EPOLLIN | EPOLLRDHUP;
    epoll_ctl(epollFD, EPOLL_CTL_ADD, sockfd, &ev);

    epoll_event events[MAX_EVENTS];

    char * msg = "Hello, world!!!\n";
    sendToFD(sockfd, msg, strlen(msg));

    for (;;) {
        int nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
        if (nfds == 1) {
            int curFD = events[0].data.fd;
            if (events[0].events & EPOLLRDHUP) {
                printf("server stopped\n", curFD);
                close(curFD);
                break;
            } else if (events[0].events & EPOLLIN) {
                char buf[500];
                int nbytes = recieveFromFD(curFD, buf, 500);
                // = 0 - hung up
                // < 0 - error with recieve
                if (nbytes > 0) {
                    buf[nbytes] = '\0';
                    printf("%s", buf);
                }
            }
        }
    }

    return 0;*/
}
