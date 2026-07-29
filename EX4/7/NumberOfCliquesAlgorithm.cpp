#include "NumberOfCliquesAlgorithm.h"
#include "GraphUtilities.h"
#include "Graph.h"
#include <numeric>
#include <sstream>

NumberOfCliquesAlgorithm::NumberOfCliquesAlgorithm()
{
}

string NumberOfCliquesAlgorithm::activate(const Graph& graph)
{
    stringstream ss;
    ss << "Total number of maximal cliques: " << findNumberOfCliques(graph);
    return ss.str();
}

int NumberOfCliquesAlgorithm::findNumberOfCliques(const Graph& graph)
{
    int n = graph.getSize();
    vector<int> R, P(n), X, dummy_clique;
    vector<vector<int>> adj_matrix(n, vector<int>(n, 0)); // Adjacency Matrix (needed for Clique and Hamilton computations)
    GraphUtilities::fillAdjMatrix(adj_matrix, graph);
    iota(P.begin(), P.end(), 0);
    int clique_count = 0;
    GraphUtilities::bronKerbosch(R, P, X, dummy_clique, clique_count, false, adj_matrix, graph);
    return clique_count;
}
