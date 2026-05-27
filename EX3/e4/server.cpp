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

const char *inet_ntop2(void *addr, char *buf, size_t size)
{
    struct sockaddr_storage *sas = (struct sockaddr_storage*)addr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;

    switch (sas->ss_family) {
        case AF_INET:
            sa4 = (struct sockaddr_in*)addr;
            src = &(sa4->sin_addr);
            break;

        case AF_INET6:
            sa6 = (struct sockaddr_in6*)addr;
            src = &(sa6->sin6_addr);
            break;

        default:
            return NULL;
    }

    return inet_ntop(sas->ss_family, src, buf, size);
}



/*
 * Return a listening socket.
 */
int get_listener_socket(void)
{
    int listener;     // Listening socket descriptor
    int yes=1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // All done with this

    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

/*
 * Add a new file descriptor to the set.
 */
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count,
        int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it
    *pfds = (pollfd*) realloc(*pfds, sizeof(**pfds) * (*fd_size));    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

/*
 * Remove a file descriptor at a given index from the set.
 */
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

/*
 * Handle incoming connections.
 */
void handle_new_connection(int listener, int *fd_count,
        int *fd_size, struct pollfd **pfds)
{
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;
    int newfd;  // Newly accept()ed socket descriptor
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr *)&remoteaddr,
            &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        add_to_pfds(pfds, newfd, fd_count, fd_size);

        printf("pollserver: new connection from %s on socket %d\n",
                inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
                newfd);
    }
}

/*
 * Handle regular client data or client hangups.
 */
void handle_client_data(int *fd_count,
        struct pollfd *pfds, int *pfd_i)
{
    char buf[256];    // Buffer for client data

    int nbytes = recv(pfds[*pfd_i].fd, buf, sizeof buf, 0);

    int sender_fd = pfds[*pfd_i].fd;

    if (nbytes <= 0) { // Got error or connection closed by client
        if (nbytes == 0) {
            // Connection closed
            printf("pollserver: socket %d hung up\n", sender_fd);
        } else {
            perror("recv");
        }

        close(pfds[*pfd_i].fd); // Bye!

        del_from_pfds(pfds, *pfd_i, fd_count);

        // reexamine the slot we just deleted
        (*pfd_i)--;

    } else { // We got some good data from a client
        printf("pollserver: recv from fd %d: %.*s\n", sender_fd,
                nbytes, buf);
        buf[nbytes] = '\0';
        string input(buf, nbytes);
        if (input.rfind("Newgraph ", 0) == 0)
        {
            int amount = 0;
            amount = stoi(input.substr(9));
            points.clear();
            points.reserve(amount);
            for (int i = 0; i < amount; i++)
            {
                nbytes = recv(pfds[*pfd_i].fd, buf, sizeof buf, 0);
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
        else if (input == "CH")
        {
            // Sort points lexicographically
            sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
                return (a.x < b.x) || (a.x == b.x && a.y < b.y);
            });

            // Build convex hull using monotonic chain
            vector<Point> hull;

            // Lower hull
            for (const auto& p : points) {
                while (hull.size() >= 2 && cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }

            // Upper hull
            size_t lowerSize = hull.size();
            for (int i = points.size() - 1; i >= 0; i--) {
                const auto& p = points[i];
                while (hull.size() > lowerSize &&
                    cross(hull[hull.size()-2], hull.back(), p) <= 0)
                    hull.pop_back();
                hull.push_back(p);
            }

            hull.pop_back(); // last point is duplicate

            // Compute area
            double area = polygonArea(hull);

            cout << "Convex Hull Area: " << area << endl;
        }
        else if (input.rfind("Newpoint ", 0) == 0)
        {
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
        else if (input.rfind("Removepoint ", 0) == 0)
        {
            bool removed = false;
            input = input.substr(12);
            replace(input.begin(), input.end(), ',', ' ');
            stringstream ss(input);
            Point p;
            if (!(ss >> p.x >> p.y)) {
                cerr << "Error: Invalid point format. Try again with: <x>,<y>\n";
                return;
            }
            for (size_t i = 0; i < points.size(); i++)
            {
                if (points[i].x == p.x && points[i].y == p.y){
                    points.erase(points.begin() + i);
                    removed = true;
                }
            }
            if (removed)
                cout << "The point is removed.\n";
            else
                cout << "The point does not exist.\n";

        }
        else
        {
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
void process_connections(int listener, int *fd_count, int *fd_size,
        struct pollfd **pfds)
{
    for(int i = 0; i < *fd_count; i++) {

        // Check if someone's ready to read
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            // We got one!!

            if ((*pfds)[i].fd == listener) {
                // If we're the listener, it's a new connection
                handle_new_connection(listener, fd_count, fd_size,
                        pfds);
            } else {
                // Otherwise we're just a regular client
                handle_client_data(fd_count, *pfds, &i);
            }
        }
    }
}

/*
 * Main: create a listener and connection set, loop forever
 * processing connections.
 */
int main(void)
{

    int listener;     // Listening socket descriptor

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    int fd_size = 5;
    int fd_count = 0;
    struct pollfd *pfds = (pollfd*) malloc(sizeof(*pfds) * fd_size);
    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the listener to set;
    // Report ready to read on incoming connection
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1; // For the listener

    puts("pollserver: waiting for connections...");

    // Main loop
    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Run through connections looking for data to read
        process_connections(listener, &fd_count, &fd_size, &pfds);
    }

    free(pfds);
}