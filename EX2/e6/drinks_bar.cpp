#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/file.h>


#define BUFFER_SIZE 1024

using namespace std;

unsigned long long carbon = 0;
unsigned long long oxygen = 0;
unsigned long long hydrogen = 0;

// Maximum atoms allowed for each type
const unsigned long long MAX_ATOMS = 1000000000000000000ULL; // 10^18

char resource_file[256] = {0};
bool use_resource = false;

// =====================================================
//                 FILE HANDLING
// =====================================================
bool update_values_in_file(const char* path)
{
    char tmp_path[512];
    snprintf(tmp_path, sizeof(tmp_path), "%s.tmp", path);

    FILE* f = fopen(tmp_path, "w");
    if (!f) return false;

    int fd = fileno(f);
    flock(fd, LOCK_EX);

    fprintf(f, "carbon=%llu\noxygen=%llu\nhydrogen=%llu\n",
            carbon, oxygen, hydrogen);

    fflush(f);
    fsync(fd);   // מבטיח שהמידע באמת נכתב לדיסק

    flock(fd, LOCK_UN);
    fclose(f);

    // החלפה אטומית של הקובץ
    rename(tmp_path, path);

    return true;
}

bool load_values_from_file(const char* path)
{
    FILE* f = fopen(path, "r");
    if (!f) {
        return update_values_in_file(path); // יצירה אוטומטית
    }

    int fd = fileno(f);
    flock(fd, LOCK_SH);

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        unsigned long long v;
        if (sscanf(line, "carbon=%llu", &v) == 1) carbon = v;
        else if (sscanf(line, "oxygen=%llu", &v) == 1) oxygen = v;
        else if (sscanf(line, "hydrogen=%llu", &v) == 1) hydrogen = v;
    }

    flock(fd, LOCK_UN);
    fclose(f);
    return true;
}


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

    if (use_resource && !load_values_from_file(resource_file)) {
        cerr << "Failed to load resource file\n";
        exit(1);
    }
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
         
    if (use_resource) update_values_in_file(resource_file);

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

    if (use_resource && !load_values_from_file(resource_file)) {
        cerr << "Failed to load resource file\n";
        exit(1);
    }
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

    if (use_resource) update_values_in_file(resource_file);
    
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
void handle_arguments(int argc, char* argv[],int* TCP_port, int* UDP_port, int* timeout,
                    char stream_socket_path[], char datagram_socket_path[],
                    bool* useTCP, bool* useSTREAM, bool* useUPD, bool* useDATAGRAM)
{
    for (int i = 1; i < argc; i++){
        string a = argv[i];

        if (a == "-o" || a == "--oxygen") oxygen = atoll(argv[i+1]);
        if (a == "-c" || a == "--carbon") carbon = atoll(argv[i+1]);
        if (a == "-h" || a == "--hydrogen") hydrogen = atoll(argv[i+1]);
        if (a == "-T" || a == "--tcp-port") {
            *TCP_port = atoi(argv[i+1]);
            *useTCP = true;
        }
        if (a == "-U" || a == "--udp-port") {
            *UDP_port = atoi(argv[i+1]);
            *useUPD = true;
        }
        if (a == "-t" || a == "--timeout") *timeout = atoi(argv[i+1]);
        if (a == "-d" || a == "--datagram-path") {
            strcpy(datagram_socket_path, argv[i+1]);
            *useDATAGRAM = true;
        }
        if (a == "-s" || a == "--stream-path") {
            strcpy(stream_socket_path, argv[i+1]);
            *useSTREAM = true;
        }
        if (a == "-f" || a =="--save-file") {
            strcpy(resource_file, argv[i+1]);
            use_resource = true;
        }
    }
    if ((*useDATAGRAM && *useUPD) || (!*useDATAGRAM && !*useUPD) 
      ||  (*useTCP && *useSTREAM) || (!*useTCP && !*useSTREAM)) 
      {
        cerr << "Usage: " << argv[0] << " -U <UDP port> -T <TCP port> [-o <oxygen amount>] " <<
            "[-c <carbon amount>]\n\t\t   [-h <hydrogen amount>] [-t <timeout in sec>]\n\t\t   " <<
            "[-s <UDS stream file path>] [-d <UDS datagram filepath>]\n\t\t   " <<
            "[-f <file path>]\n\n"
            "-U, --hydrogen <UDP port>\t\t\tset port for UDP \n" <<
            "-T, --tcp-port <TCP port>\t\t\tset port for TCP \n" <<
            "-o, --oxygen <oxygen amount>\t\t\tset amount of oxygen\n" <<
            "-c, --carbon <carbon amount>\t\t\tset amount of carbon\n" <<
            "-h, --hydrogen <hydrogen amount>\t\tset amount of hydrogen\n" <<
            "-t, --timeout <sec amount>\t\t\tset timeout, when the time up without geting message, the program will be ended\n" <<
            "-s, --stream-path <UDS stream file path>\tthe path for stream file (required this flag or -U)\n" << 
            "-d, --datagram-path <UDS datagram filepath>\tthe path for datagram file (required this flag or -T)\n" <<
            "-f, --save-file <path to file>\t\t\tpath to file that save all the data\n";
        exit(1);
    }

}

// =====================================================
//                        MAIN
// =====================================================
int main(int argc, char* argv[]) 
{
    int TCP_port = 0,UDP_port = 0, timeout = 0, activity;
    fd_set master_set, read_fds;
    char buffer[BUFFER_SIZE];
    char stream_socket_path[100];
    char datagram_socket_path[100];
    bool useTCP = false, useSTREAM = false, useDATAGRAM = false, useUDP = false;

    handle_arguments(argc, argv, &TCP_port, &UDP_port, &timeout,
                     stream_socket_path, datagram_socket_path,
                     &useTCP, &useSTREAM,&useUDP, &useDATAGRAM);

    if (use_resource) {
        if (!load_values_from_file(resource_file)) {
            cerr << "Failed to load resource file\n";
            return 1;
        }
        cout << "Loaded values from file: "
             << "C=" << carbon << " O=" << oxygen << " H=" << hydrogen << endl;
    }
    // =====================================================
    //                   TIMEOUT (if chosen)
    // =====================================================
        
    struct timeval tv;
    struct timeval* ptv = nullptr;
    if (timeout > 0) {
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        ptv = &tv;
    }

    // =====================================
    //             STREAM SERVER 
    // =====================================
    int Stream_server_fd = -1;

    if (useSTREAM) {
        Stream_server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (Stream_server_fd < 0) {
            perror("STREAM socket");
            return 1;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, stream_socket_path);
        unlink(stream_socket_path);

        if (bind(Stream_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("STREAM bind");
            return 1;
        }

        if (listen(Stream_server_fd, 5) < 0) {
            perror("STREAM listen");
            return 1;
        }

        cout << "STREAM server ready.\n";
    }

    // =====================================
    //               TCP SERVER 
    // =====================================
    int TCP_server_fd = -1;

    if (useTCP) {

        if (TCP_port <= 0 || TCP_port > 65535) {
            std::cerr << "Invalid TCP port number\n";
            return 1;
        }

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
    }

    // ======================================
    //               UDP SERVER 
    // ======================================
    int UDP_server_fd = -1;

    if (useUDP) {
        UDP_server_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (UDP_server_fd < 0) {
            perror("UDP socket");
            return 1;
        }
        int opt = 1;
        setsockopt(UDP_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in UDP_addr{};
        UDP_addr.sin_family = AF_INET;
        UDP_addr.sin_addr.s_addr = INADDR_ANY;
        UDP_addr.sin_port = htons(UDP_port);

        if (bind(UDP_server_fd, (sockaddr*)&UDP_addr, sizeof(UDP_addr)) < 0) {
            perror("UDP bind");
            return 1;
        }
    }
    // ========================================
    //              DATAGRAM SERVER 
    // ========================================
    int DATAGRAM_server_fd = -1;

    if (useDATAGRAM) {
        DATAGRAM_server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (DATAGRAM_server_fd < 0) {
            perror("DATAGRAM socket");
            return 1;
        }

        sockaddr_un daddr{};
        daddr.sun_family = AF_UNIX;
        strcpy(daddr.sun_path, datagram_socket_path);
        unlink(datagram_socket_path);

        if (bind(DATAGRAM_server_fd, (sockaddr*)&daddr, sizeof(daddr)) < 0) {
            perror("DATAGRAM bind");
            return 1;
        }

        cout << "DATAGRAM server ready.\n";
    }

    // =====================================================
    //                 SELECT SETUP
    // =====================================================
    FD_ZERO(&master_set);
    FD_SET(STDIN_FILENO, &master_set);

    int max_fd = STDIN_FILENO;

    if (useUDP) {
        FD_SET(UDP_server_fd, &master_set);
        max_fd = max(max_fd, UDP_server_fd);
    }
    if (useDATAGRAM) {
        FD_SET(DATAGRAM_server_fd, &master_set);
        max_fd = max(max_fd, DATAGRAM_server_fd);
    }
    if (useTCP) {
        FD_SET(TCP_server_fd, &master_set);
        max_fd = max(max_fd, TCP_server_fd);
    }

    if (useSTREAM) {
        FD_SET(Stream_server_fd, &master_set);
        max_fd = max(max_fd, Stream_server_fd);
    }

    // =====================================================
    //                 MAIN LOOP
    // =====================================================
    while (true) {
        read_fds = master_set;

        activity = select(max_fd + 1, &read_fds, nullptr, nullptr, ptv);
        if (activity < 0) {
            perror("select");
            break;
        }
        if (activity == 0) {
            cout << "Timeout\nExiting...\n";
            return 0;
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
                else if (cmd == "print") {
                    cout << "CARBON = " << carbon
                         << ", OXYGEN = " << oxygen
                         << ", HYDROGEN = " << hydrogen << endl; 
                }
                else
                    Keyboard_handle_command(cmd);
                continue;
            }
            
            // ---------------- STREAM SERVER ----------------
            if (useSTREAM && fd == Stream_server_fd) {

                int client = accept(Stream_server_fd, nullptr, nullptr);
                if (client < 0) {
                    perror("STREAM accept");
                    continue;
                }

                FD_SET(client, &master_set);
                max_fd = max(max_fd, client);
                continue;
            }

            // ---------------- TCP SERVER ----------------
            if (useTCP && fd == TCP_server_fd) {
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
            if (useUDP && fd == UDP_server_fd) {
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

            // ---------------- DATAGRAM SERVER ----------------
            if (useDATAGRAM && fd == DATAGRAM_server_fd) {
                sockaddr_un cli;
                socklen_t len = sizeof(cli);
                int bytes = recvfrom(fd, buffer, BUFFER_SIZE - 1, 0,
                                    (sockaddr*)&cli, &len);

                if (bytes > 0) {
                    buffer[bytes] = '\0';
                    string resp = UDP_handle_command(buffer);
                    sendto(fd, resp.c_str(), resp.size(), 0,
                        (sockaddr*)&cli, len);
                }
                continue;
            }

            // ---------------- STREAM CLIENT DATA ----------------
            if (useSTREAM && fd != Stream_server_fd) {

                int bytes = recv(fd, buffer, BUFFER_SIZE - 1, 0);

                if (bytes <= 0) {
                    close(fd);
                    FD_CLR(fd, &master_set);
                    continue;
                }

                buffer[bytes] = '\0';
                string cmd = buffer;

                TCP_handle_command(cmd);
                
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
        if (timeout > 0)
            ptv->tv_sec = timeout;
    }

    return 0;
}