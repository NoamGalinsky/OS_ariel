
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0]
             << " \"name\"" << endl;
        return 1;
    }

    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();

    if (pid1 == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid1 == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        char* grepArgs[] = {
            (char*)"grep",
            argv[1],
            (char*)"phonebook.txt",
            nullptr
        };

        execvp("grep", grepArgs);

        perror("execvp grep");
        _exit(1);
    }

    pid_t pid2 = fork();

    if (pid2 == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid2 == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        char* cutArgs[] = {
            (char*)"cut",
            (char*)"-d",
            (char*)",",
            (char*)"-f2",
            nullptr
        };

        execvp("cut", cutArgs);

        perror("execvp cut");
        _exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    return 0;
}
