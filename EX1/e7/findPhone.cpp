#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;
/*
 * findPhone <name>
 *
 * Searches phonebook.txt for the phone number of the given name.
 * Implements the shell pipeline:
 *   grep "name" phonebook.txt | sed 's/ /#/g' | sed 's/,/ /' | awk '{print$2}'
 *
 * Uses only fork(2), execvp(2), pipe(2), dup2(2) — no strings API.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        write(STDERR_FILENO, "Usage: findPhone <name>\n", 23);
        return 1;
    }

    /* Three pipes connecting four processes */
    int p1[2], p2[2], p3[2];
    pipe(p1);
    pipe(p2);
    pipe(p3);

    /* ── Process 1: grep argv[1] phonebook.txt ──────────────────────── */
    if (fork() == 0) {
        dup2(p1[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        const char *args[] = {"grep", argv[1], "phonebook.txt", nullptr};
        execvp("grep", const_cast<char* const*>(args));
        _exit(1);
    }

    /* ── Process 2: sed 's/ /#/g'  (replace spaces → #) ────────────── */
    if (fork() == 0) {
        dup2(p1[0], STDIN_FILENO);
        dup2(p2[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        const char *args[] = {"sed", "s/ /#/g", nullptr};
        execvp("sed", const_cast<char* const*>(args));
        _exit(1);
    }

    /* ── Process 3: sed 's/,/ /'   (replace comma → space) ─────────── */
    if (fork() == 0) {
        dup2(p2[0], STDIN_FILENO);
        dup2(p3[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        const char *args[] = {"sed", "s/,/ /", nullptr};
        execvp("sed", const_cast<char* const*>(args));
        _exit(1);
    }

    /* ── Process 4: awk '{print$2}'  (print phone field) ───────────── */
    if (fork() == 0) {
        dup2(p3[0], STDIN_FILENO);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        const char *args[] = {"awk", "{print$2}", nullptr};
        execvp("awk", const_cast<char* const*>(args));
        _exit(1);
    }

    /* Parent closes all pipe ends so children receive EOF correctly */
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);

    while (wait(nullptr) > 0);
    return 0;
}
