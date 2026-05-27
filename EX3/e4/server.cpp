#define _POSIX_C_SOURCE 200112L

#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

#define PORT "9034"   // Port we're listening on

using namespace std;

struct Point {
    double x, y;
};

static vector<Point> points;

// Cross product to determine orientation
double cross(const Point& O, const Point& A, const Point& B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

// Compute polygon area using Shoelace formula
double polygonArea(const vector<Point>& hull) {
    double area = 0;
    int n = hull.size();
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        area += hull[i].x * hull[j].y - hull[j].x * hull[i].y;
    }
    return fabs(area) / 2.0;
}

/*
 * Convert socket to IP address string.
 * addr: struct sockaddr_in or struct sockaddr_in6
 */
const char* inet_ntop2(void* addr, char* buf, size_t size) {
    struct sockaddr_storage* sas = (struct sockaddr_storage*)addr;
    void* src;
    switch (sas->ss_family) {
        case AF_INET:  src = &((struct sockaddr_in*)addr)->sin_addr;  break;
        case AF_INET6: src = &((struct sockaddr_in6*)addr)->sin6_addr; break;
        default: return nullptr;
    }
    return inet_ntop(sas->ss_family, src, buf, size);
}

/*
 * Return a listening socket.
 */
int get_listener_socket() {
    int listener, yes = 1, rv;
    struct addrinfo hints{}, *ai, *p;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(nullptr, PORT, &hints, &ai)) != 0) {
        cerr << "server: " << gai_strerror(rv) << '\n';
        exit(1);
    }
    for (p = ai; p != nullptr; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    if (p == nullptr) return -1;
    freeaddrinfo(ai);
    if (listen(listener, 10) == -1) return -1;
    return listener;
}

/*
 * Add a new file descriptor to the set.
 */
void add_to_pfds(vector<pollfd>& pfds, int newfd) {
    pfds.push_back({newfd, POLLIN, 0});
}

/*
 * Remove a file descriptor at a given index from the set.
 */
void del_from_pfds(vector<pollfd>& pfds, int i) {
    pfds[i] = pfds.back();
    pfds.pop_back();
}

/*
 * Handle incoming connections.
 */
void handle_new_connection(int listener, vector<pollfd>& pfds) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];

    int newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
    if (newfd == -1) {
        cerr << "accept: " << strerror(errno) << '\n';
    } else {
        add_to_pfds(pfds, newfd);
        cout << "server: new connection from "
             << inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP)
             << " on socket " << newfd << '\n';
    }
}

/*
 * Handle regular client data or client hangups.
 */
void handle_client_data(vector<pollfd>& pfds, int& pfd_i) {
    char buf[256];
    int sender_fd = pfds[pfd_i].fd;
    int nbytes = recv(sender_fd, buf, sizeof buf, 0);

    if (nbytes <= 0) {
        if (nbytes == 0)
            cout << "server: socket " << sender_fd << " hung up\n";
        else
            cerr << "recv: " << strerror(errno) << '\n';
        close(sender_fd);
        del_from_pfds(pfds, pfd_i);
        pfd_i--; // reexamine the slot we just deleted
    } else {
        buf[nbytes] = '\0';
        string input(buf, nbytes);
        cout << "server: recv from fd " << sender_fd << ": " << input << '\n';

        if (input.rfind("Newgraph ", 0) == 0) {
            int amount = stoi(input.substr(9));
            points.clear();
            points.reserve(amount);
            for (int i = 0; i < amount; i++) {
                nbytes = recv(sender_fd, buf, sizeof buf, 0);
                if (nbytes <= 0) continue;
                buf[nbytes] = '\0';
                input = string(buf, nbytes);
                replace(input.begin(), input.end(), ',', ' ');
                stringstream ss(input);
                Point p;
                if (!(ss >> p.x >> p.y)) {
                    cerr << "Error: Invalid point format. Try again with: <x>,<y>\n";
                    continue;
                }
                points.push_back(p);
            }
            cout << "The graph is built.\n";
        }
        else if (input == "CH") {
            sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
                return (a.x < b.x) || (a.x == b.x && a.y < b.y);
            });
            vector<Point> hull;
            for (const auto& p : points) {
                while (hull.size() >= 2 && cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }
            size_t lowerSize = hull.size();
            for (int i = (int)points.size() - 1; i >= 0; i--) {
                const auto& p = points[i];
                while (hull.size() > lowerSize &&
                    cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }
            hull.pop_back(); // last point is duplicate
            cout << "Convex Hull Area: " << polygonArea(hull) << endl;
        }
        else if (input.rfind("Newpoint ", 0) == 0) {
            input = input.substr(9);
            replace(input.begin(), input.end(), ',', ' ');
            stringstream ss(input);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format. Try again with: <x>,<y>\n";
                return;
            }
            points.push_back(p);
            cout << "The point is added.\n";
        }
        else if (input.rfind("Removepoint ", 0) == 0) {
            bool removed = false;
            input = input.substr(12);
            replace(input.begin(), input.end(), ',', ' ');
            stringstream ss(input);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format. Try again with: <x>,<y>\n";
                return;
            }
            for (size_t i = 0; i < points.size(); i++) {
                if (points[i].x == p.x && points[i].y == p.y) {
                    points.erase(points.begin() + i);
                    removed = true;
                }
            }
            cout << (removed ? "The point is removed.\n" : "The point does not exist.\n");
        }
        else {
            cerr << "USAGE:\n"
                 << "  Newgraph <n>        - create new graph, then enter n lines of: <x>,<y>\n"
                 << "  CH                  - compute convex hull area\n"
                 << "  Newpoint <x>,<y>    - add a point\n"
                 << "  Removepoint <x>,<y> - remove a point\n"
                 << "  ctrl+D              - close\n";
        }
    }
}

/*
 * Process all existing connections.
 */
void process_connections(int listener, vector<pollfd>& pfds) {
    for (int i = 0; i < (int)pfds.size(); i++) {
        if (pfds[i].revents & (POLLIN | POLLHUP)) {
            if (pfds[i].fd == listener)
                handle_new_connection(listener, pfds);
            else
                handle_client_data(pfds, i);
        }
    }
}

/*
 * Main: create a listener and connection set, loop forever
 * processing connections.
 */
int main() {
    int listener = get_listener_socket();
    if (listener == -1) {
        cerr << "error getting listening socket\n";
        exit(1);
    }

    vector<pollfd> pfds;
    pfds.push_back({listener, POLLIN, 0});

    cout << "server: waiting for connections...\n";

    for (;;) {
        int poll_count = poll(pfds.data(), pfds.size(), -1);
        if (poll_count == -1) {
            cerr << "poll: " << strerror(errno) << '\n';
            exit(1);
        }
        process_connections(listener, pfds);
    }
}
