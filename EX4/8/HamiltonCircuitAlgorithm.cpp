#include "HamiltonCircuitAlgorithm.h"
#include "Graph.h"
#include "GraphUtilities.h"
#include <sstream>

HamiltonCircuitAlgorithm::HamiltonCircuitAlgorithm()
{
}

string HamiltonCircuitAlgorithm::activate(const Graph& graph)
{
    vector<int> ham_circ = findHamiltonCircuit(graph);
    stringstream ss;
    if (!ham_circ.empty())
    {
        ss << "Hamiltonian Circuit: ";
        for (int node : ham_circ)
        {
            ss << node << " ";
        }
    }
    else
    {
        ss << "No Hamiltonian Circuit exists.";
    }
    return ss.str();
}

vector<int> HamiltonCircuitAlgorithm::findHamiltonCircuit(const Graph& graph)
{
    int n = graph.getSize();
    vector<vector<int>> adj_matrix(n, vector<int>(n, 0)); // Adjacency Matrix (needed for Clique and Hamilton computations)
    vector<int> path;
    GraphUtilities::fillAdjMatrix(adj_matrix, graph);
    path.push_back(0); // Always start circuit from Node 0
    if (hamiltonianDFS(0, 1, path, adj_matrix, graph))
    {
        return path;
    }
    return {};
}

// Helper for Hamiltonian Circuit (Backtracking with Bitmask DP style validation)
bool HamiltonCircuitAlgorithm::hamiltonianDFS(int u, int mask, vector<int>& path, vector<vector<int>>& adj_matrix, const Graph& graph)
{
    int n = graph.getSize();
    if (mask == (1 << n) - 1)
    {
        // Check if there is an edge back to the starting node (0)
        if (adj_matrix[u][0])
        {
            path.push_back(0);
            return true;
        }
        return false;
    }

    for (int v = 0; v < n; ++v)
    {
        if (adj_matrix[u][v] && !(mask & (1 << v)))
        {
            path.push_back(v);
            if (hamiltonianDFS(v, mask | (1 << v), path, adj_matrix, graph))
            {
                return true;
            }
            path.pop_back(); // Backtrack
        }
    }
    return false;
}
