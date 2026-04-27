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
//                 REMOVE ATOMS FOR MOLECULES
// =====================================================
bool remove_atom(unsigned int amount, int type)
{
    switch(type){
        case 0: // WATER H2O
            if (hydrogen >= amount*2 && oxygen >= amount) {
                hydrogen -= amount*2;
                oxygen -= amount;
                return true;
            }
            return false;

        case 1: // CO2
            if (carbon >= amount && oxygen >= amount*2) {
                carbon -= amount;
                oxygen -= amount*2;
                return true;
            }
            return false;

        case 2: // ALCOHOL C2H6O
            if (carbon >= amount*2 && hydrogen >= amount*6 && oxygen >= amount) {
                carbon -= amount*2;
                hydrogen -= amount*6;
                oxygen -= amount;
                return true;
            }
            return false;

        case 3: // GLUCOSE C6H12O6
            if (carbon >= amount*6 && hydrogen >= amount*12 && oxygen >= amount*6) {
                carbon -= amount*6;
                hydrogen -= amount*12;
                oxygen -= amount*6;
                return true;
            }
            return false;
    }
    return false;
}

// =====================================================
//                 UDP COMMAND HANDLER
// =====================================================
string UDP_handle_command(const string& cmd)
{
    size_t pos;
    string num;
    unsigned int amount = 0;
    bool valid = true;
    bool enough = true;

    if (cmd.rfind("DELIVER WATER ", 0) == 0) {
        num = cmd.substr(14);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (!remove_atom(amount, 0)) enough = false;
    }
    else if (cmd.rfind("DELIVER CARBON DIOXIDE ", 0) == 0) {
        num = cmd.substr(23);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (!remove_atom(amount, 1)) enough = false;
    }
    else if (cmd.rfind("DELIVER ALCOHOL ", 0) == 0) {
        num = cmd.substr(16);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (!remove_atom(amount, 2)) enough = false;
    }
    else if (cmd.rfind("DELIVER GLUCOSE ", 0) == 0) {
        num = cmd.substr(16);
        amount = stoi(num, &pos);
        if (pos != num.size()) valid = false;
        else if (!remove_atom(amount, 3)) enough = false;
    }
    else valid = false;

    if (!valid) {
        cout << "ERROR: invalid command\n";
        return "ERROR: invalid command";
    }
    if (!enough) {
        cout << "ERROR: The warehouse does not have enough atoms\n";
        return "ERROR: The warehouse does not have enough atoms";
    }

    cout << "CARBON = " << carbon
         << " OXYGEN = " << oxygen
         << " HYDROGEN = " << hydrogen << endl;

    return "The command was accepted";
}

// =====================================================
//                 KEYBOARD COMMANDS
// =====================================================
void Keyboard_handle_command(const string& cmd)
{
    int counter = 0;

    if (cmd == "GEN SOFT DRINK") {
        counter = carbon / 7;
        counter = std::min(counter, (int)(hydrogen / 14));
        counter = std::min(counter, (int)(oxygen / 9));
        cout << "The bar has " << counter << " SOFT DRINK\n";
    }
    else if (cmd == "GEN VODKA") {
        counter = carbon / 8;
        counter = std::min(counter, (int)(hydrogen / 20));
        counter = std::min(counter, (int)(oxygen / 8));
        cout << "The bar has " << counter << " VODKA\n";
    }
    else if (cmd == "GEN CHAMPAGNE") {
        counter = carbon / 3;
        counter = std::min(counter, (int)(hydrogen / 8));
        counter = std::min(counter, (int)(oxygen / 4));
        cout << "The bar has " << counter << " CHAMPAGNE\n";
    }
    else cout << "Invalid command\n";
}

// =====================================================
//                 ARGUMENT HANDLING
// =====================================================
void handle_arguments(int argc, char* argv[],int* TCP_port, int* UDP_port)
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <TCP port>" << endl;;
        exit(1);
    }
    *TCP_port = atoi(argv[1]);
    *UDP_port = atoi(argv[2]);
}

// =====================================================
//                        MAIN
// =====================================================
int main(int argc, char* argv[]) 
{
    int TCP_port = 0,UDP_port = 0, max_fd = 0;
    fd_set master_set, read_fds;
    char buffer[BUFFER_SIZE];

    handle_arguments(argc, argv, &TCP_port, &UDP_port);


    // =====================================
    //                 TCP SERVER 
    // =====================================
    if (TCP_port <= 0 || TCP_port > 65535) {
        std::cerr << "Invalid TCP port number\n";
        return 1;
    }
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

    // ======================================
    //                 UDP SERVER 
    // ======================================
    int UDP_server_fd = -1;

    UDP_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_server_fd < 0) {
        perror("UDP socket");
        return 1;
    }

    setsockopt(UDP_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in UDP_addr{};
    UDP_addr.sin_family = AF_INET;
    UDP_addr.sin_addr.s_addr = INADDR_ANY;
    UDP_addr.sin_port = htons(UDP_port);

    if (bind(UDP_server_fd, (sockaddr*)&UDP_addr, sizeof(UDP_addr)) < 0) {
        perror("UDP bind");
        return 1;
    }
    // =====================================================
    //                 SELECT SETUP
    // =====================================================
    FD_ZERO(&master_set);
    FD_SET(TCP_server_fd, &master_set);
    FD_SET(UDP_server_fd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);
    max_fd = max(max_fd, TCP_server_fd);
    max_fd = max(max_fd, UDP_server_fd);
    max_fd = max(max_fd, STDIN_FILENO);


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

            // ---------------- KEYBOARD ----------------
            if (fd == STDIN_FILENO) {
                string cmd;
                getline(cin, cmd);
                if (cmd == "quit") {
                    cout << "Exiting...\n";
                    return 0;
                }
                Keyboard_handle_command(cmd);
                continue;
            }
            
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

            // ---------------- UDP SERVER ----------------
            if (fd == UDP_server_fd) {
                sockaddr_in cli;
                socklen_t len = sizeof(cli);
                int bytes = recvfrom(fd, buffer, BUFFER_SIZE - 1, 0,(sockaddr*)&cli, &len);

                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    string resp = UDP_handle_command(buffer);

                    sendto(fd, resp.c_str(), resp.size(), 0,
                        (sockaddr*)&cli, len);
                }
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