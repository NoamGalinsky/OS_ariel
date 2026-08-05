#pragma once
#include "GraphAlgorithm.h"
#include <vector>

using namespace std;

// Disjoint Set Union (DSU) helper structure for Kruskal's MST
struct DSU
{
    vector<int> parent, rank;
    DSU(int n);
    int find(int i);
    bool unite(int i, int j);
};

// class for activating the MST weight algorithm
class MSTWeightAlgorithm :
    public GraphAlgorithm
{
public:
    MSTWeightAlgorithm();
    virtual string activate(const Graph& graph);

private:
    // Finding MST Weight using Kruskal's Algorithm
    int findMstWeight(const Graph& graph);
};

