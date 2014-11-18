//#include <QCoreApplication>
#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

int main() {
    server myServer("127.0.0.1", "2323", 10);
    myServer.start();
    return 0;
}
