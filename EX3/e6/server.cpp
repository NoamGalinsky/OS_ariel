#define _POSIX_C_SOURCE 200112L

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <csignal>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

#include "reactor.hpp"

#define PORT "9034"

using namespace std;

struct Point {
    double x, y;
};

static vector<Point> points;
static void* g_reactor = nullptr;
static volatile sig_atomic_t g_running = 1;

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

const char* inet_ntop2(void* addr, char* buf, size_t size) {
    struct sockaddr_storage* sas = (struct sockaddr_storage*)addr;
    void* src;
    if (sas->ss_family == AF_INET)
        src = &((struct sockaddr_in*)addr)->sin_addr;
    else if (sas->ss_family == AF_INET6)
        src = &((struct sockaddr_in6*)addr)->sin6_addr;
    else
        return nullptr;
    return inet_ntop(sas->ss_family, src, buf, size);
}

int get_listener_socket() {
    int listener, yes = 1, rv;
    struct addrinfo hints{}, *ai, *p;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    if (p == NULL) return -1;
    freeaddrinfo(ai);
    if (listen(listener, 10) == -1) return -1;
    return listener;
}

void* handleClient(int fd) {
    char buf[256];
    int nbytes = recv(fd, buf, sizeof(buf) - 1, 0);

    if (nbytes <= 0) {
        if (nbytes == 0)
            printf("server: socket %d hung up\n", fd);
        else
            perror("recv");
        close(fd);
        removeFdFromReactor(g_reactor, fd);
        return nullptr;
    }

    buf[nbytes] = '\0';
    string input(buf, nbytes);

    if (input.rfind("Newgraph ", 0) == 0) {
        int amount = stoi(input.substr(9));
        points.clear();
        points.reserve(amount);
        for (int i = 0; i < amount; i++) {
            nbytes = recv(fd, buf, sizeof(buf) - 1, 0);
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
            while (hull.size() > lowerSize && cross(hull[hull.size()-2], hull.back(), p) <= 0)
                hull.pop_back();
            hull.push_back(p);
        }
        hull.pop_back();
        cout << "Convex Hull Area: " << polygonArea(hull) << endl;
    }
    else if (input.rfind("Newpoint ", 0) == 0) {
        input = input.substr(9);
        replace(input.begin(), input.end(), ',', ' ');
        stringstream ss(input);
        Point p;
        if (!(ss >> p.x >> p.y)) {
            cerr << "Error: Invalid point format. Try again with: <x>,<y>\n";
            return nullptr;
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
            return nullptr;
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
    return nullptr;
}

void* handleNewConnection(int listener) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    char remoteIP[INET6_ADDRSTRLEN];

    int newfd = accept(listener, (struct sockaddr*)&remoteaddr, &addrlen);
    if (newfd == -1) {
        perror("accept");
        return nullptr;
    }
    printf("server: new connection from %s on socket %d\n",
           inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP), newfd);
    addFdToReactor(g_reactor, newfd, handleClient);
    return nullptr;
}

void sigHandler(int) {
    g_running = 0;
}

int main() {
    signal(SIGINT, sigHandler);

    int listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    g_reactor = startReactor();
    if (!g_reactor) {
        fprintf(stderr, "error starting reactor\n");
        exit(1);
    }

    addFdToReactor(g_reactor, listener, handleNewConnection);
    puts("server: waiting for connections...");

    while (g_running) pause();

    stopReactor(g_reactor);
    close(listener);
    return 0;
}
