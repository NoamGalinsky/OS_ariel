#include "ClientHandler.h"
#include "GraphAlgorithmFactory.h"
#include "ActiveObject.h"
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
    string algNames[] = {"hamilton", "max", "mst", "num"};
    GraphAlgorithmFactory* factory = GraphAlgorithmFactory::getGraphAlgorithmFactory();
    activeObject = nullptr;
    ActiveObject* currentActiveObject = nullptr;
    int i = 0;
    for (string algName : algNames)
    {
        GraphAlgorithm* algo = factory->getGraphAlgorithm(algName);
        if (algo != nullptr)
        {
            ActiveObject* tempActiveObject = new ActiveObject(algo);
            if (activeObject == nullptr)
            {
                activeObject = tempActiveObject;
            }
            if (currentActiveObject != nullptr)
            {
                currentActiveObject->setNext(tempActiveObject);
            }
            currentActiveObject = tempActiveObject;
        }
        i++;
    }
}

ClientHandler::~ClientHandler()
{
    delete clientData;
    ActiveObject* currentActiveObject = activeObject;
    while (currentActiveObject != nullptr)
    {
        ActiveObject* tempActiveObject = currentActiveObject->getNext();
        delete currentActiveObject;
        currentActiveObject = tempActiveObject;
    }
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

string ClientHandler::activate()
{
    ActiveObjectArgs args;
    args.graph = &graph;
    args.clientHandler = this;
    thread t(ClientHandler::activateAlgo, &args);
    t.join();
    return args.ret;
}

void ClientHandler::activateActiveObject(void *ptr)
{
    ActiveObjectArgs* params = (ActiveObjectArgs *)ptr;
    activeObject->activate(*params);
}

void* ClientHandler::activateAlgo(void *ptr)
{
    ActiveObjectArgs* params = (ActiveObjectArgs *)ptr;
    params->clientHandler->activateActiveObject(ptr);
    pthread_exit(NULL);
}

