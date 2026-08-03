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
#include <thread>
#include <iostream>

using namespace std;
#define MAX_RANDOM_WEIGHT 20
#define GRAPH_ALGO_NUM 4
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
    if (strcmp(commandName, "random") == 0)
    {
        return random();
    }
    if (strcmp(commandName, "activate") == 0)
    {
        return activate();
    }
    if (strcmp(commandName, "exit") == 0)
    {
        return "";
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

string Server::random()
{
    char* token = strtok(NULL, " ");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int seed = atoi(token);
    srand(seed);
    token = strtok(NULL, " ");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int vertices = atoi(token);
    graph.clean(vertices);
    token = strtok(NULL, "");
    if (token == NULL)
    {
        return "Number is missing";
    }
    int edges = atoi(token);
    for (int i = 0; i < edges; i++)
    {
        graph.addEdge(rand() % vertices, rand() % vertices, rand() % MAX_RANDOM_WEIGHT);
    }
    // show the graph in the server side
    cout << graph << endl;
    return "Generated a random graph";
}

// parameter for the thread that runs one algorithm
typedef struct {
    GraphAlgorithm* algo; // Parameter 1
    Graph* graph;   // Parameter 2
    string ret;     // return value
} ActivateAlgoArgs;

string Server::activate()
{
    static string algNames[] = {"hamilton", "max", "mst", "num"};
    thread* threads[GRAPH_ALGO_NUM];
    GraphAlgorithmFactory* factory = GraphAlgorithmFactory::getGraphAlgorithmFactory();
    ActivateAlgoArgs args[GRAPH_ALGO_NUM];
    int i = 0;
    // create the threads
    for (string algName : algNames)
    {
        GraphAlgorithm* algo = factory->getGraphAlgorithm(algName);
        if (algo != nullptr)
        {
            args[i].algo = algo;
            args[i].graph = &graph;
            threads[i] = new thread(Server::activateAlgo, &args[i]);
        }
        i++;
    }
    // wait for the threads to finish
    for (thread* thread : threads)
    {
        thread->join();
    }
    for (thread* thread : threads)
    {
        delete thread;
    }
    // collect the answers to one long answer to be sent to the client
    string str;
    for (ActivateAlgoArgs arg : args)
    {
        str += arg.ret + "\n";
    }
    return str;
}

void* Server::activateAlgo(void *ptr)
{
    ActivateAlgoArgs* params = (ActivateAlgoArgs *)ptr;
    params->ret = params->algo->activate(*params->graph);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
    Server server;
    server.run();
    return 0;
}

