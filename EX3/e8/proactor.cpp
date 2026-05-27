#include "proactor.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdio>

struct Proactor {
    int sockfd;
    proactorFunc func;
    volatile bool running;
};

// wrapper to adapt proactorFunc (int fd) → pthread_create's void* (*)(void*)
struct ClientArg {
    proactorFunc func;
    int fd;
};

static Proactor* g_proactor = nullptr;

static void* clientWrapper(void* arg) {
    auto* ca = static_cast<ClientArg*>(arg);
    proactorFunc func = ca->func;
    int fd = ca->fd;
    delete ca;
    return func(fd);
}

static void* proactorLoop(void* arg) {
    auto* p = static_cast<Proactor*>(arg);

    while (p->running) {
        struct sockaddr_storage remoteaddr;
        socklen_t addrlen = sizeof(remoteaddr);

        int newfd = accept(p->sockfd, (struct sockaddr*)&remoteaddr, &addrlen);
        if (newfd < 0) {
            if (p->running) perror("accept");
            break;
        }

        auto* ca = new ClientArg{p->func, newfd};
        pthread_t tid;
        if (pthread_create(&tid, nullptr, clientWrapper, ca) != 0) {
            perror("pthread_create");
            close(newfd);
            delete ca;
            continue;
        }
        pthread_detach(tid);
    }

    return nullptr;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    auto* p = new Proactor{sockfd, threadFunc, true};

    pthread_t tid;
    if (pthread_create(&tid, nullptr, proactorLoop, p) != 0) {
        perror("pthread_create");
        delete p;
        return 0;
    }

    g_proactor = p;
    return tid;
}

int stopProactor(pthread_t tid) {
    if (g_proactor == nullptr) return -1;
    Proactor* p = g_proactor;
    g_proactor = nullptr;

    p->running = false;
    shutdown(p->sockfd, SHUT_RDWR);  // ensures accept() in the proactor thread is interrupted
    close(p->sockfd);
    pthread_join(tid, nullptr);
    delete p;
    return 0;
}
