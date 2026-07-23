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
        cerr << "Usage: " << argv[0] << " <server ip> <port>\n";
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);

    // =======================
    //          TCP
    // =======================
    if (port <= 0 || port > 65535) {
        cerr << "Invalid port number\n";
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
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

    // Connect to server
    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect failed");
        close(sock);
        return 1;
    }

    cout << "Connected to server " << server_ip << ":" << port << endl;
    // =======================
    //      Command loop
    // =======================
    while(true)
    {
        // Read command from user
        string cmd;
        
        cout << endl << "To close the connection enter \"close\"" << endl
             << "Enter command: ";
        getline(cin, cmd);
        if (cmd == "close")
            break;

        // Send command to server
        if (send(sock, cmd.c_str(), cmd.size(), 0) < 0) {
            perror("send");
            break;
        }
    }
    close(sock);
    return 0;
}