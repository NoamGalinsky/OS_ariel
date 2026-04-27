#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>

#define BUFFER_SIZE 1024
#define CLIENT_PATH "/tmp/atom_client.sock"

using namespace std;

// =====================================================
//                 ARGUMENT HANDLING
// =====================================================
void handle_arguments(int argc, char* argv[], int* port, char server_ip[], char datagram_socket_path[])
{
    bool port_flag = false;
    bool ip_flag = false;
    bool datagram_flag = false;

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
            strcpy(datagram_socket_path, argv[++i]);
            datagram_flag = true;
        }
    }

    if (!((ip_flag && port_flag && !datagram_flag) ||
          (!ip_flag && !port_flag && datagram_flag))) 
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
    char datagram_socket_path[100] = {0};
    char buffer[BUFFER_SIZE];
    sockaddr_in serv_addr{};

    handle_arguments(argc, argv, &port, server_ip, datagram_socket_path);

    // =========================
    //      UNIX DATAGRAM
    // =========================
    if (port == -1) {

        sock = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock < 0) {
            perror("socket");
            return 1;
        }

        // bind client socket (required for reply)
        unlink(CLIENT_PATH);

        sockaddr_un client_addr{};
        client_addr.sun_family = AF_UNIX;

        strncpy(client_addr.sun_path,CLIENT_PATH,sizeof(client_addr.sun_path) - 1);

        if (bind(sock,(sockaddr*)&client_addr,sizeof(client_addr)) < 0) 
        {
            perror("bind");
            close(sock);
            return 1;
        }

        cout << "Connected using UNIX DATAGRAM\n"
             << "Server path: " << datagram_socket_path << endl;
    }

    // =========================
    //           UDP
    // =========================
    else
    {
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
    }
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

        // -------- UNIX DATAGRAM --------
        if (port == -1) {

            sockaddr_un server_addr{};
            server_addr.sun_family = AF_UNIX;
            strncpy(server_addr.sun_path,
                    datagram_socket_path,
                    sizeof(server_addr.sun_path) - 1);
                    
            if (sendto(sock, cmd.c_str(), cmd.size(), 0,
                       (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                perror("sendto");
                break;
            }
        }

        // -------- UDP --------
        else {
            // Send command to server
            if (sendto(sock, cmd.c_str(), cmd.size(), 0, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
            {
                perror("send");
                break;
            }

        }

        // -------- Receive response ----------
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
