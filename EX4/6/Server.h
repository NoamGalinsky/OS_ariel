#pragma once

#include <string>
#include "Graph.h"

using namespace std;

class Server
{
public:
    Server();
    void run();
private:
    Graph graph;
    string handleCommand(char* command);
    string add();
    string clean();
};

