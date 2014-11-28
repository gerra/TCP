#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <functional>

#include <iostream>

int main() {
    try {
        server myServer("127.0.0.1", "2323", 10);
        myServer.onAccept = [](TCPSocket &sock) {
            char buf[500];
            sock.recieveMsg(buf, 500);
            std::cout << buf;
            sock.sendMsg(buf);
            if (buf[0] == 'T') {
                sock.closeSocket();
            }
            throw 5;
        };
        myServer.start();
    } catch (TCPException e) {
        std::cout << e.getMessage() << "\n";
    }
    std::cout << "Finish\n";
    return 0;
}
