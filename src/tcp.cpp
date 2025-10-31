#include "tcp.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int tcpBind(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = ntohs(PORT);

    if (bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    return fd;
}

void tcpWritetoClient(int fd) {
    if (listen(fd, SOMAXCONN) == -1) {
        perror("listen");
        exit(1);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    std::string wbuf = "Hello, world\n";

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (true) {
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
        if (connfd == -1) {
            continue;
        }

        write(connfd, wbuf.c_str(), wbuf.length());
        close(connfd);
    }
}
