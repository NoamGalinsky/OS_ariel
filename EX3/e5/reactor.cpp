#include "reactor.hpp"
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <poll.h>
#include <unistd.h>
#include <cstdio>

struct Reactor {
    std::unordered_map<int, reactorFunc> handlers;
    std::mutex mtx;
    std::thread thread;
    bool running;
    int wakePipe[2];
};

static void reactorLoop(Reactor* r) {
    while (r->running) {
        std::vector<struct pollfd> pfds;
        {
            std::lock_guard<std::mutex> lock(r->mtx);
            pfds.push_back({r->wakePipe[0], POLLIN, 0});
            for (auto& [fd, func] : r->handlers)
                pfds.push_back({fd, POLLIN, 0});
        }

        int ret = poll(pfds.data(), (nfds_t)pfds.size(), -1);
        if (ret < 0) break;
        if (ret == 0) continue;

        if (pfds[0].revents & POLLIN) {
            char buf[64];
            [[maybe_unused]] ssize_t n = read(r->wakePipe[0], buf, sizeof(buf));
        }

        for (size_t i = 1; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                reactorFunc func = nullptr;
                {
                    std::lock_guard<std::mutex> lock(r->mtx);
                    auto it = r->handlers.find(pfds[i].fd);
                    if (it != r->handlers.end())
                        func = it->second;
                }
                if (func) func(pfds[i].fd);
            }
        }
    }
}

void* startReactor() {
    Reactor* r = new Reactor();
    r->running = true;
    if (pipe(r->wakePipe) == -1) {
        delete r;
        return nullptr;
    }
    r->thread = std::thread(reactorLoop, r);
    return r;
}

int addFdToReactor(void* reactor, int fd, reactorFunc func) {
    Reactor* r = static_cast<Reactor*>(reactor);
    {
        std::lock_guard<std::mutex> lock(r->mtx);
        r->handlers[fd] = func;
    }
    [[maybe_unused]] ssize_t n = write(r->wakePipe[1], "w", 1);
    return 0;
}

int removeFdFromReactor(void* reactor, int fd) {
    Reactor* r = static_cast<Reactor*>(reactor);
    {
        std::lock_guard<std::mutex> lock(r->mtx);
        r->handlers.erase(fd);
    }
    [[maybe_unused]] ssize_t n = write(r->wakePipe[1], "w", 1);
    return 0;
}

int stopReactor(void* reactor) {
    Reactor* r = static_cast<Reactor*>(reactor);
    r->running = false;
    [[maybe_unused]] ssize_t n = write(r->wakePipe[1], "w", 1);
    r->thread.join();
    close(r->wakePipe[0]);
    close(r->wakePipe[1]);
    delete r;
    return 0;
}
