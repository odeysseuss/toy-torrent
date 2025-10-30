#include "tcp.hpp"

int main(void) {
    int fd = tcpBind();
    tcpWritetoClient(fd);

    return 0;
}
