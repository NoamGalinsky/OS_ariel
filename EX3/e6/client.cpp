#define _POSIX_C_SOURCE 200112L

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <iostream>

int main()
{
    const char *server_ip = "127.0.0.1";
    const char *port = "9034";

    int sockfd;
    struct addrinfo hints{}, *res, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    int rv;
    if ((rv = getaddrinfo(server_ip, port, &hints, &res)) != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
        exit(1);
    }

    // Connect to first possible result
    for (p = res; p != nullptr; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
            continue;

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }

        break; // Success
    }

    if (p == nullptr) {
        std::cerr << "Failed to connect.\n";
        exit(2);
    }

    freeaddrinfo(res);

    std::cout << "Connected to " << server_ip << ":" << port << "\n";
    std::cout << "Type messages and press Enter to send.\n";

    std::string input;
    std::string input2;

    while (std::getline(std::cin, input)) {
        send(sockfd, input.c_str(), input.size(), 0);
    }

    close(sockfd);
    return 0;
}
