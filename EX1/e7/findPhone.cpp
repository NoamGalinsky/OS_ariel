
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0]
             << " <name or partial name>" << endl;
        return 1;
    }

    string searchName;

    for (int i = 1; i < argc; i++)
    {
        searchName += argv[i];

        if (i != argc - 1)
            searchName += " ";
    }

    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t grep_pid = fork();

    if (grep_pid == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        char* grepArgs[] =
        {
            (char*)"grep",
            (char*)searchName.c_str(),
            (char*)"phonebook.txt",
            nullptr
        };

        execvp("grep", grepArgs);

        perror("grep");
        _exit(1);
    }

    pid_t cut_pid = fork();

    if (cut_pid == 0)
    {
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        char* cutArgs[] =
        {
            (char*)"cut",
            (char*)"-d",
            (char*)",",
            (char*)"-f2",
            nullptr
        };

        execvp("cut", cutArgs);

        perror("cut");
        _exit(1);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(grep_pid, nullptr, 0);
    waitpid(cut_pid, nullptr, 0);

    return 0;
}
