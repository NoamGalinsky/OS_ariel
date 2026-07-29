#pragma once

#include <string>

using namespace std;

class GraphAlgorithm;
class Graph;
class Server;

struct ActiveObjectArgs
{
    Graph* graph;   // Parameter 1
    Server* server;   // Parameter 2
    string ret;     // return value
};

class ActiveObject
{
public:
    ActiveObject(GraphAlgorithm* algo);
    void activate(ActiveObjectArgs&);
    void setNext(ActiveObject* next);
    ActiveObject* getNext() const;

private:
    GraphAlgorithm* algo;
    ActiveObject* next;
};

