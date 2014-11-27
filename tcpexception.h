#ifndef TCPEXCEPTION_H
#define TCPEXCEPTION_H

#include <string>

class TCPException : std::exception {
private:
    std::string msg;
public:
    TCPException(std::string msg) : msg(msg) {}
    TCPException(const char *msg) : msg(msg) {}
    std::string getMessage() {
        return msg;
    }
};
#endif // TCPEXCEPTION_H
