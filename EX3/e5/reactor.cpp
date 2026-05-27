#include "reactor.hpp"
#include <unordered_map>
#include <vector>
#include <pthread.h>
#include <poll.h>
#include <unistd.h>
#include <iostream>
#include <cerrno>
#include <cstring>

struct Reactor {
    std::unordered_map<int, reactorFunc> handlers;
    pthread_mutex_t mtx;
    pthread_t thread;
    bool running;
    int wakePipe[2];
};

static void* reactorLoop(void* arg) {
    Reactor* r = static_cast<Reactor*>(arg);
    while (r->running) {
        std::vector<struct pollfd> pfds;
        pthread_mutex_lock(&r->mtx);
        pfds.push_back({r->wakePipe[0], POLLIN, 0});
        for (auto& [fd, func] : r->handlers)
            pfds.push_back({fd, POLLIN, 0});
        pthread_mutex_unlock(&r->mtx);

        int ret = poll(pfds.data(), (nfds_t)pfds.size(), -1);
        if (ret < 0) break;
        if (ret == 0) continue;

        if (pfds[0].revents & POLLIN) {
            char buf[64];
            (void)read(r->wakePipe[0], buf, sizeof(buf));
        }

        for (size_t i = 1; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                reactorFunc func = nullptr;
                pthread_mutex_lock(&r->mtx);
                auto it = r->handlers.find(pfds[i].fd);
                if (it != r->handlers.end())
                    func = it->second;
                pthread_mutex_unlock(&r->mtx);
                if (func) func(pfds[i].fd);
            }
        }
    }
    return nullptr;
}

void* startReactor() {
    Reactor* r = new Reactor();
    r->running = true;
    if (pthread_mutex_init(&r->mtx, nullptr) != 0) {
        delete r;
        return nullptr;
    }
    if (pipe(r->wakePipe) == -1) {
        pthread_mutex_destroy(&r->mtx);
        delete r;
        return nullptr;
    }
    if (pthread_create(&r->thread, nullptr, reactorLoop, r) != 0) {
        close(r->wakePipe[0]);
        close(r->wakePipe[1]);
        pthread_mutex_destroy(&r->mtx);
        delete r;
        return nullptr;
    }
    return r;
}

int addFdToReactor(void* reactor, int fd, reactorFunc func) {
    Reactor* r = static_cast<Reactor*>(reactor);
    pthread_mutex_lock(&r->mtx);
    r->handlers[fd] = func;
    pthread_mutex_unlock(&r->mtx);
    (void)write(r->wakePipe[1], "w", 1);
    return 0;
}

int removeFdFromReactor(void* reactor, int fd) {
    Reactor* r = static_cast<Reactor*>(reactor);
    pthread_mutex_lock(&r->mtx);
    r->handlers.erase(fd);
    pthread_mutex_unlock(&r->mtx);
    (void)write(r->wakePipe[1], "w", 1);
    return 0;
}

int stopReactor(void* reactor) {
    Reactor* r = static_cast<Reactor*>(reactor);
    r->running = false;
    (void)write(r->wakePipe[1], "w", 1);
    pthread_join(r->thread, nullptr);
    close(r->wakePipe[0]);
    close(r->wakePipe[1]);
    pthread_mutex_destroy(&r->mtx);
    delete r;
    return 0;
}
