#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#define BUFFER_SIZE 1024

using namespace std;

unsigned long long carbon = 0;
unsigned long long oxygen = 0;
unsigned long long hydrogen = 0;

// Maximum atoms allowed for each type
const unsigned long long MAX_ATOMS = 1000000000000000000ULL; // 10^18

// =====================================================
//                 TCP COMMAND HANDLER
// =====================================================
void TCP_handle_command(const string& cmd)
{
    size_t pos;
    string num;
    unsigned int amount = 0;
    bool valid = true;
    bool limit = true;
    string type;

    if (cmd.rfind("ADD CARBON ", 0) == 0) {
        type = "CARBON";
        num = cmd.substr(11);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (carbon + amount > MAX_ATOMS) limit = false;
        else carbon += amount;
    }
    else if (cmd.rfind("ADD OXYGEN ", 0) == 0) {
        type = "OXYGEN";
        num = cmd.substr(11);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (oxygen + amount > MAX_ATOMS) limit = false;
        else oxygen += amount;
    }
    else if (cmd.rfind("ADD HYDROGEN ", 0) == 0) {
        type = "HYDROGEN";
        num = cmd.substr(13);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (hydrogen + amount > MAX_ATOMS) limit = false;
        else hydrogen += amount;
    }
    else valid = false;

    if (!valid) {
        cout << "Error: Invalid command\n";
        return;
    }
    if (!limit) {
        cout << "ERROR: Amount of " << type << " above the allowed limit\n";
        return;
    }

    cout << "CARBON = " << carbon
         << ", OXYGEN = " << oxygen
         << ", HYDROGEN = " << hydrogen << endl;
}
// =====================================================
//                 ARGUMENT HANDLING
// =====================================================
void handle_arguments(int argc, char* argv[],int* TCP_port)
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <TCP port>" << endl;;
        exit(1);
    }
    *TCP_port = atoi(argv[1]);
}

// =====================================================
//                        MAIN
// =====================================================
int main(int argc, char* argv[]) {
    int TCP_port = 0, max_fd = 0;
    fd_set master_set, read_fds;
    char buffer[BUFFER_SIZE];

    handle_arguments(argc, argv, &TCP_port);

    if (TCP_port <= 0 || TCP_port > 65535) {
        std::cerr << "Invalid TCP port number\n";
        return 1;
    }
    // =====================================================
    //                 TCP SERVER (if chosen)
    // =====================================================
    int TCP_server_fd = -1;

    TCP_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (TCP_server_fd < 0) {
        perror("TCP socket");
        return 1;
    }

    int opt = 1;
    setsockopt(TCP_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in TCP_addr{};
    TCP_addr.sin_family = AF_INET;
    TCP_addr.sin_addr.s_addr = INADDR_ANY;
    TCP_addr.sin_port = htons(TCP_port);

    if (bind(TCP_server_fd, (sockaddr*)&TCP_addr, sizeof(TCP_addr)) < 0) {
        perror("TCP bind");
        return 1;
    }

    if (listen(TCP_server_fd, 5) < 0) {
        perror("TCP listen");
        return 1;
    }

    cout << "TCP server listening on port " << TCP_port << endl;

    // =====================================================
    //                 SELECT SETUP
    // =====================================================
    FD_ZERO(&master_set);
    FD_SET(TCP_server_fd, &master_set);
    max_fd = max(max_fd, TCP_server_fd);

    // =====================================================
    //                 MAIN LOOP
    // =====================================================
    while (true) {
        read_fds = master_set;

        if (select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr) < 0) {
            perror("select failed");
            break;
        }

        for (int fd = 0; fd <= max_fd; fd++) {
            if (!FD_ISSET(fd, &read_fds)) continue;

            // ---------------- TCP SERVER ----------------
            if (fd == TCP_server_fd) {
                sockaddr_in cli;
                socklen_t len = sizeof(cli);
                int client = accept(TCP_server_fd, (sockaddr*)&cli, &len);

                if (client < 0) {
                    perror("TCP accept");
                    continue;
                }

                FD_SET(client, &master_set);
                max_fd = max(max_fd, client);
                continue;
            }


            // ---------------- TCP CLIENT DATA ----------------
            int bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);

            if (bytes <= 0) {
                close(fd);
                FD_CLR(fd, &master_set);
            }
            else {
                buffer[bytes] = '\0';
                TCP_handle_command(buffer);
            }
        }
    }

    return 0;
}