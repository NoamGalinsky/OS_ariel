#pragma once

#include <string>

using namespace std;

class GraphAlgorithm;
class Graph;
class Server;

// the parameter for active object
struct ActiveObjectArgs
{
    Graph* graph;   // Parameter 1
    Server* server;   // Parameter 2
    string ret;     // return value
};

// class for active object
class ActiveObject
{
public:
    ActiveObject(GraphAlgorithm* algo);
    // activate the algorithm and then call the next one
    void activate(ActiveObjectArgs&);
    void setNext(ActiveObject* next);
    ActiveObject* getNext() const;

private:
    // the algorithm to be activated
    GraphAlgorithm* algo;
    // the nest active object to be activate
    ActiveObject* next;
};

