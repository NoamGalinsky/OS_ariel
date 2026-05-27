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
#include <pthread.h>
#include <csignal>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cmath>

#define PORT "9034"

using namespace std;

struct Point {
    double x, y;
};

static vector<Point> points;
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static volatile sig_atomic_t g_running = 1;
static int g_listener = -1;

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
        cerr << "getaddrinfo: " << gai_strerror(rv) << '\n';
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

// Receives one newline-terminated line from fd into buf (up to size-1 chars).
// Returns number of bytes read, or <=0 on error/close.
int recvLine(int fd, char* buf, int size) {
    int nbytes = recv(fd, buf, size - 1, 0);
    if (nbytes <= 0) return nbytes;
    buf[nbytes] = '\0';
    // trim trailing \r\n
    while (nbytes > 0 && (buf[nbytes-1] == '\n' || buf[nbytes-1] == '\r'))
        buf[--nbytes] = '\0';
    return nbytes;
}

void* handleClient(void* arg) {
    int fd = *(int*)arg;
    free(arg);

    char buf[256];

    while (true) {
        int nbytes = recvLine(fd, buf, sizeof buf);
        if (nbytes <= 0) {
            if (nbytes == 0)
                cout << "server: socket " << fd << " hung up\n";
            else
                cerr << "recv: " << strerror(errno) << '\n';
            break;
        }

        string input(buf);

        if (input.rfind("Newgraph ", 0) == 0) {
            int amount = stoi(input.substr(9));
            vector<Point> newPoints;
            newPoints.reserve(amount);
            for (int i = 0; i < amount; i++) {
                nbytes = recvLine(fd, buf, sizeof buf);
                if (nbytes <= 0) continue;
                string line(buf);
                replace(line.begin(), line.end(), ',', ' ');
                stringstream ss(line);
                Point p;
                if (!(ss >> p.x >> p.y)) {
                    cerr << "Error: Invalid point format\n";
                    continue;
                }
                newPoints.push_back(p);
            }
            pthread_mutex_lock(&g_mutex);
            points = move(newPoints);
            pthread_mutex_unlock(&g_mutex);
            cout << "The graph is built.\n";

        } else if (input == "CH") {
            // Copy points under lock so another thread can't modify mid-sort
            pthread_mutex_lock(&g_mutex);
            vector<Point> pts = points;
            pthread_mutex_unlock(&g_mutex);

            sort(pts.begin(), pts.end(), [](const Point& a, const Point& b) {
                return (a.x < b.x) || (a.x == b.x && a.y < b.y);
            });

            vector<Point> hull;
            for (const auto& p : pts) {
                while (hull.size() >= 2 && cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }
            size_t lowerSize = hull.size();
            for (int i = (int)pts.size() - 1; i >= 0; i--) {
                const auto& p = pts[i];
                while (hull.size() > lowerSize && cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }
            if (!hull.empty()) hull.pop_back();

            cout << "Convex Hull Area: " << polygonArea(hull) << endl;

        } else if (input.rfind("Newpoint ", 0) == 0) {
            string coords = input.substr(9);
            replace(coords.begin(), coords.end(), ',', ' ');
            stringstream ss(coords);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format\n";
                continue;
            }
            pthread_mutex_lock(&g_mutex);
            points.push_back(p);
            pthread_mutex_unlock(&g_mutex);
            cout << "The point is added.\n";

        } else if (input.rfind("Removepoint ", 0) == 0) {
            string coords = input.substr(12);
            replace(coords.begin(), coords.end(), ',', ' ');
            stringstream ss(coords);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format\n";
                continue;
            }
            bool removed = false;
            pthread_mutex_lock(&g_mutex);
            for (size_t i = 0; i < points.size(); i++) {
                if (points[i].x == p.x && points[i].y == p.y) {
                    points.erase(points.begin() + i);
                    removed = true;
                    break;
                }
            }
            pthread_mutex_unlock(&g_mutex);
            cout << (removed ? "The point is removed.\n" : "The point does not exist.\n");

        } else {
            cerr << "USAGE:\n"
                 << "  Newgraph <n>        - create new graph, then enter n lines of: <x>,<y>\n"
                 << "  CH                  - compute convex hull area\n"
                 << "  Newpoint <x>,<y>    - add a point\n"
                 << "  Removepoint <x>,<y> - remove a point\n"
                 << "  ctrl+D              - close\n";
        }
    }

    close(fd);
    return nullptr;
}

void sigHandler(int) {
    g_running = 0;
    // Wake up accept() so the main loop can exit
    if (g_listener != -1) close(g_listener);
}

int main() {
    signal(SIGINT, sigHandler);

    g_listener = get_listener_socket();
    if (g_listener == -1) {
        cerr << "error getting listening socket\n";
        exit(1);
    }

    cout << "server: waiting for connections...\n";

    while (g_running) {
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof remoteaddr;
        char remoteIP[INET6_ADDRSTRLEN];

        int newfd = accept(g_listener, (struct sockaddr*)&remoteaddr, &addrlen);
        if (newfd == -1) {
            if (g_running) cerr << "accept: " << strerror(errno) << '\n';
            break;
        }

        cout << "server: new connection from "
             << inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP)
             << " on socket " << newfd << '\n';

        int* fdPtr = (int*)malloc(sizeof(int));
        *fdPtr = newfd;

        pthread_t tid;
        if (pthread_create(&tid, nullptr, handleClient, fdPtr) != 0) {
            cerr << "pthread_create: " << strerror(errno) << '\n';
            close(newfd);
            free(fdPtr);
            continue;
        }
        // Detach so the thread cleans itself up when done
        pthread_detach(tid);
    }

    pthread_mutex_destroy(&g_mutex);
    return 0;
}
