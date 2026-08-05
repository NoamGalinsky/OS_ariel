#pragma once

#include "GraphAlgorithm.h"
#include <vector>

using namespace std;

// class for activating the Hamilton circuit algorithm
class HamiltonCircuitAlgorithm :
    public GraphAlgorithm
{
public:
    HamiltonCircuitAlgorithm();
    virtual string activate(const Graph& graph);

private:
    // Finding Hamilton Circuit using Backtracking with Bitmasking
    vector<int> findHamiltonCircuit(const Graph& graph);
    // Helper for Hamiltonian Circuit (Backtracking with Bitmask DP style validation)
    bool hamiltonianDFS(int u, int mask, vector<int>& path, vector<vector<int>>& adj_matrix, const Graph& graph);
};
