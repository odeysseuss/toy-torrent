#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8000

int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(1);
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in serv_addr, client_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = ntohs(PORT);

    if (bind(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(fd, SOMAXCONN) == -1) {
        perror("listen");
        exit(1);
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    std::string wbuf = "Hello, world";

    while (true) {
        socklen_t client_len = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
        if (connfd == -1) {
            continue;
        }

        write(connfd, wbuf.c_str(), wbuf.length());
        close(connfd);
    }

    return 0;
}
