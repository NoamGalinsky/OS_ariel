#include "MaxCliqueAlgorithm.h"
#include "GraphUtilities.h"
#include "Graph.h"
#include <numeric>
#include <sstream>

MaxCliqueAlgorithm::MaxCliqueAlgorithm()
{
}

string MaxCliqueAlgorithm::activate(const Graph& graph)
{
    vector<int> mc = findMaxClique(graph);
    stringstream ss;
    ss << "Maximal Clique size: " << mc.size() << " | Elements: ";
    for (int node : mc)
    {
        ss << node << " ";
    }
    return ss.str();
}

vector<int> MaxCliqueAlgorithm::findMaxClique(const Graph& graph)
{
    int n = graph.getSize();
    vector<int> R, P(n), X, max_clique;
    vector<vector<int>> adj_matrix(n, vector<int>(n, 0)); // Adjacency Matrix (needed for Clique and Hamilton computations)
    GraphUtilities::fillAdjMatrix(adj_matrix, graph);
    iota(P.begin(), P.end(), 0); // Populate P with 0 to n-1
    int dummy_count = 0;
    GraphUtilities::bronKerbosch(R, P, X, max_clique, dummy_count, true, adj_matrix, graph);
    return max_clique;
}
