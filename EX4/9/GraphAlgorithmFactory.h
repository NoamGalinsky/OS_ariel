#pragma once

#include <string>
#include <map>

using namespace std;

// factory to get a graph algorithm by its name
class GraphAlgorithm;

class GraphAlgorithmFactory
{
public:
    // return the singleton object
    static GraphAlgorithmFactory* getGraphAlgorithmFactory();
    GraphAlgorithm* getGraphAlgorithm(const string& name);

private:
    // the constructor is private to privent creating an object
    GraphAlgorithmFactory();
    // map from an algorithm name to an algorithm
    map<string, GraphAlgorithm*> graphAlgorithms;
};

