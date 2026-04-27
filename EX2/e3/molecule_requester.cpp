#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

using namespace std;

int main(int argc, char* argv[]) {
    int sock;
    addrinfo* res = nullptr;
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <port>\n";
        return 1;
    }
    const char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // =========================
    //           UDP
    // =========================
    if (port <= 0 || port > 65535) {
        cerr << "Invalid port number\n";
        return 1;
    }
    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket failed");
        return 1;
    }

    // Server address
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Try to resolve IP or hostname
    if (getaddrinfo(server_ip, NULL, NULL, &res) != 0) {
        cerr << "Invalid IP or hostname\n";
        close(sock);
        return 1;
    }

    sockaddr_in* ipv4 = (sockaddr_in*)res->ai_addr;
    serv_addr.sin_addr = ipv4->sin_addr;
    freeaddrinfo(res);

    cout << "Connected using UDP\n"
            << "Server: " << server_ip << ":" << port << endl;

    // =========================
    //       Command loop
    // =========================
    while (true) 
    {
        // Read command from user
        string cmd;

        cout << endl << "To close the connection enter \"close\"" << endl
             << "Enter command: ";
        getline(cin, cmd);
        if (cmd == "close")
            break;

        // Send command to server
        if (sendto(sock, cmd.c_str(), cmd.size(), 0, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
        {
            perror("send");
            break;
        }

        // Receive response
        char buffer[BUFFER_SIZE];
        sockaddr_in from_addr{};
        socklen_t from_len = sizeof(from_addr);

        int bytes = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0, (sockaddr*)&from_addr, &from_len);

        if (bytes < 0) {
            perror("recvfrom failed");
            continue;
        }
        buffer[bytes] = '\0';
        cout << "Server: " << buffer << endl;
    }

    close(sock);
    return 0;
}
