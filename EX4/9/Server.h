#pragma once

#include <string>
#include "Graph.h"

using namespace std;

class ActiveObject;

class Server
{
public:
    Server();
    ~Server();
    void run();
private:
    Graph graph;
    string handleCommand(char* command);
    string add();
    string addw();
    string clean();
    string random();
    string activate();
    void activateActiveObject(void *ptr);
    static void* activateAlgo(void *ptr);
    ActiveObject* activeObject;
};

