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
    // implement command to add a new edge with weight
    string addw();
    // implement command to reset the graph to a new size
    string clean();
    // implement command to generate a random graph
    string random();
    // implement command to activate the all algorithms on the graph
    string activate();
    // function to activate the an algorithm on the graph in a different thread
    static void* activateAlgo(void *ptr);
};

