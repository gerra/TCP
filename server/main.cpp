//#include <QCoreApplication>
#include "server.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

int main(int argc, char *argv[]) {
    server myServer("127.0.0.1", "2323");
    myServer.start();
    return 0;
}
