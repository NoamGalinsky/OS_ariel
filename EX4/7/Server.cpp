#include "Server.h"
#include "EulerAlgorithm.h"
#include "GraphAlgorithmFactory.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <iostream>

using namespace std;
#define PORT 3490
#define BACKLOG 10 /* how many pending
connections queue
will hold */
#define BUF_SIZE 200

Server::Server() : graph(0)
{
}

void Server::run()
{
    char buf[BUF_SIZE];
    size_t nbytes = BUF_SIZE - 1;
    // listen on sock_fd, new connection on new_fd
    int sockfd, new_fd;
    // my address
    struct sockaddr_in my_addr;
    // connector addr
    struct sockaddr_in their_addr;
    socklen_t  sin_size;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0))==-1)
    {
        perror("socket");
        exit(1);
    }
    // host byte order
    my_addr.sin_family = AF_INET;
    // short, network byte order
    my_addr.sin_port = htons(PORT);
    /* INADDR_ANY allows clients to connect to any one of
    the host’s IP address */
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // zero the struct
    bzero(&(my_addr.sin_zero), 8);
    if (bind(sockfd, (struct sockaddr *)&my_addr,
        sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    // main accept() loop
    while(1)
    {
        sin_size = sizeof(struct sockaddr_in);
        if ((new_fd = accept(sockfd, (struct sockaddr*)
            &their_addr,&sin_size)) == -1)
        {
            perror("accept");
            continue;
        }
        cout << "server: got connection from " <<
            inet_ntoa(their_addr.sin_addr) << endl;
        buf[0] = '\0';
        while(strcmp(buf, "exit") != 0)
        {
            int r = recv (new_fd, buf, nbytes, 0);
            if (r >= 0)
            {
                string answer = handleCommand(buf);
                send (new_fd, answer.c_str(), answer.length() + 1, 0);
            }
        }
        close(new_fd);
    }
}

string Server::handleCommand(char* command)
{
    char* commandName = strtok(command, " ");
    if (commandName == NULL)
    {
        return "";
    }
    if (strcmp(commandName, "add") == 0)
    {
        return add();
    }
    if (strcmp(commandName, "addw") == 0)
    {
        return addw();
    }
    if (strcmp(commandName, "clean") == 0)
    {
        return clean();
    }
    if (strcmp(commandName, "exit") == 0)
    {
        return "";
    }
    GraphAlgorithmFactory* factory = GraphAlgorithmFactory::getGraphAlgorithmFactory();
    GraphAlgorithm* alg = factory->getGraphAlgorithm(commandName);
    if (alg != nullptr)
    {
        return alg->activate(graph);
    }
    return "Command not found";
}

string Server::add()
{
    char* token = strtok(NULL, " ");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int from = atoi(token);
    token = strtok(NULL, "");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int to = atoi(token);
    graph.addEdge(from, to);
    return "Added a new edge";
}

string Server::addw()
{
    char* token = strtok(NULL, " ");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int from = atoi(token);
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int to = atoi(token);
    token = strtok(NULL, "");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int weight = atoi(token);
    graph.addEdge(from, to, weight);
    return "Added a new edge";
}

string Server::clean()
{
    char* token = strtok(NULL, "");
    if (token != NULL)
    {
        int n = atoi(token);
        graph.clean(n);
        return "Graph cleaned";
    }
    return "Number is missing";
}

int main(int argc, char* argv[])
{
    Server server;
    server.run();
    return 0;
}

