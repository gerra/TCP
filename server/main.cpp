//#include <QCoreApplication>
#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <functional>

#include <iostream>
#include <vector>
std::vector<int> a(10, 10);

int main() {
    try {
        server myServer("127.0.0.1", "2323", 10);
        myServer.start();
    } catch (ERRORS e) {
        std::cout << getStringByError(e) << "\n";
    }
    return 0;
}
