#include "server.h"

server::server() {
}


server::server(char * addr, char * port) {
    tcpConnection.createAddress(addr, port);
    listener = tcpConnection.createBindingSocket();
    tcpConnection.startListening(10);
}

void server::start() {
    fd_set master;
    fd_set readFDs;
    FD_ZERO(&master);
    FD_ZERO(&readFDs);

    FD_SET(listener, &master);

    int fdMax;
    fdMax = listener;

    for (;;) {
        readFDs = master;
        if (select(fdMax + 1, &readFDs, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(SELECT_ERROR);
        }

        for (int i = 0; i <= fdMax; i++) {
            if (FD_ISSET(i, &readFDs)) {
                if (i == listener) {
                    sockaddr_storage theirAddr;
                    socklen_t addrLen;
                    addrLen = sizeof(theirAddr);
                    int newFD = accept(listener, (sockaddr *)&theirAddr, &addrLen);
                    if (newFD == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newFD, &master);
                        if (newFD > fdMax) {
                            fdMax = newFD;
                        }
                    }
                } else {
                    char buf[500];
                    int nbytes = recieveFromFD(i, buf);
                    if (nbytes <= 0) { // socket hang up or error
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        std::set<int> ex;
                        ex.insert(listener);
                        ex.insert(i);
                        sendToAllFromFDSet(fdMax, &master, buf, nbytes, &ex);
                    }
                }
            }
        }
    }
}
