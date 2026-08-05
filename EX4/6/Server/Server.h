#pragma once

#include <string>
#include "Graph.h"

using namespace std;

// class for the server
class Server
{
public:
    Server();
    void run();
private:
    // the graph that the server is working on
    Graph graph;
    // handle a command that the server receive from the client
    string handleCommand(char* command);
    // implement command to add a new edge
    string add();
    // implement command to reset the graph to a new size
    string clean();
};

