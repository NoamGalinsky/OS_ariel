#include "ClientHandler.h"
#include "GraphAlgorithmFactory.h"
#include "GraphAlgorithm.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <thread>

using namespace std;

#define BUF_SIZE 200
#define GRAPH_ALGO_NUM 4
#define MAX_RANDOM_WEIGHT 20

ClientData::ClientData(int sockfd)
{
    this->sockfd = sockfd;
}

int ClientData::getSockFd() const
{
    return sockfd;
}

ClientHandler::ClientHandler(ClientData* clientData) : graph(0)
{
    this->clientData = clientData;
}

ClientHandler::~ClientHandler()
{
    delete clientData;
}

void* ClientHandler::handleClient(void* clientData)
{
    ClientHandler clientHandler((ClientData*)clientData);
    clientHandler.handle();
    return NULL;
}

void ClientHandler::handle()
{
    size_t nbytes = BUF_SIZE - 1;
    char buf[BUF_SIZE];
    int new_fd = clientData->getSockFd();
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

string ClientHandler::handleCommand(char* command)
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

string ClientHandler::add()
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

string ClientHandler::addw()
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

string ClientHandler::clean()
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

string ClientHandler::random()
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

string ClientHandler::activate()
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
            threads[i] = new thread(ClientHandler::activateAlgo, &args[i]);
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

void* ClientHandler::activateAlgo(void *ptr)
{
    ActivateAlgoArgs* params = (ActivateAlgoArgs *)ptr;
    params->ret = params->algo->activate(*params->graph);
    pthread_exit(NULL);
}

