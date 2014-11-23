//#include <QCoreApplication>
#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <functional>

#include <iostream>

int main() {
    server myServer("192.168.0.216", "2323", 10);
    myServer.start();
    std::function<void()> a([]() {
        std::cout << "void()\n";
    });
    std::function<int(int)> b([](int x = 0) -> int {
        std::cout << "int(int)\n" << x << "\n";
        return 23 + x;
    });
    void * aa = &a;
    void * bb = &b;


    typedef std::function<void(int)> FV;

    FV x = *(static_cast<FV *>(aa));
    x(5);
    //x();

    return 0;
}
