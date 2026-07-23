#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024

using namespace std;

// =====================================================
//                 ARGUMENT HANDLING
// =====================================================
void handle_arguments(int argc, char* argv[], int* port, char server_ip[], char stream_socket_path[])
{
    bool port_flag = false;
    bool ip_flag = false;
    bool stream_flag = false;

    for (int i = 1; i < argc; i++) 
    {
        if (string(argv[i]) == "-h" && i + 1 < argc) {
            strcpy(server_ip, argv[++i]);
            ip_flag = true;
        }
        else if (string(argv[i]) == "-p" && i + 1 < argc) {
            *port = atoi(argv[++i]);
            port_flag = true;
        }
        else if (string(argv[i]) == "-f" && i + 1 < argc) {
            strcpy(stream_socket_path, argv[++i]);
            stream_flag = true;
        }
    }

    if (!((ip_flag && port_flag && !stream_flag) ||
          (!ip_flag && !port_flag && stream_flag))) 
          {
        cerr << "Usage: " << argv[0] << " -h <hostname/IP> -p <port> [-f <UDS socket file path>]\n" << 
            "\t-h <hostname/IP>\tset ip of server or hostname (required)\n" <<
            "\t-p <port>\t\tset port of server (required)\n" <<
            "\t-f <UDS socket file path>\tset or -f flag with path to socket or -p and -h\n";
        exit(1);
    }
}

// =====================================================
//                        MAIN
// =====================================================
int main(int argc, char* argv[]) {
    int sock, port = -1;
    addrinfo* res = nullptr;
    char server_ip[100] = {0};
    char stream_socket_path[100] = {0};

    handle_arguments(argc, argv, &port, server_ip, stream_socket_path);

    // =========================
    //       UNIX STREAM
    // =========================
    if (port == -1) {

        sock = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            return 1;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, stream_socket_path, sizeof(addr.sun_path) - 1);

        socklen_t len = sizeof(addr.sun_family) + strlen(addr.sun_path);

        if (connect(sock, (sockaddr*)&addr, len) < 0) {
            perror("connect");
            close(sock);
            return 1;
        }

        cout << "Connected to Unix socket: "
             << stream_socket_path << endl;
    }

    // =======================
    //          TCP
    // =======================
    else
    {
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
    }
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