#pragma once
#include "GraphAlgorithm.h"
#include <vector>

using namespace std;

// class for activating the Max clique algorithm
class MaxCliqueAlgorithm :
    public GraphAlgorithm
{
public:
    MaxCliqueAlgorithm();
    virtual string activate(const Graph& graph);

private:
    // Finding Maximum Clique using Bron-Kerbosch
    vector<int> findMaxClique(const Graph& graph);
};

