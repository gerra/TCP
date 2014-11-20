//#include <QCoreApplication>
#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <functional>

#include <iostream>

void k(void* a) {
    typedef std::function<void()>* FV;
    FV f = static_cast<FV>(a);
    (f->operator())();
}

int main() {
    server myServer("127.0.0.1", "2323", 10);
    myServer.start();
/*
    std::function<void()> a;
    int y = 7;
    a = std::function<void()>([&y]() {
        printf("lambda %d\n", y);
    });
    y = 9;
    k(&a);
*/
    return 0;
}
