#include "GraphUtilities.h"
#include "Graph.h"
#include <algorithm>

void GraphUtilities::fillAdjMatrix(vector<vector<int>>& adj_matrix, const Graph& graph)
{
    const vector<Edge>& edges = graph.getEdges();
    for (const auto& edge : edges)
    {
        adj_matrix[edge.from][edge.to] = 1;
        adj_matrix[edge.to][edge.from] = 1; // Assuming undirected graphs for Clique/MST/Hamilton
    }
}

// Helper for Bron-Kerbosch Algorithm (Cliques)
void GraphUtilities::bronKerbosch(vector<int>& R, vector<int>& P, vector<int>& X,
    vector<int>& max_clique, int& clique_count, bool find_max_only,
    vector<vector<int>>& adj_matrix, const Graph& graph)
{
    if (P.empty() && X.empty())
    {
        clique_count++;
        if (find_max_only && R.size() > max_clique.size())
        {
            max_clique = R;
        }
        return;
    }

    vector<int> P_copy = P;
    for (int v : P_copy)
    {
        // Build new sets intersecting with Neighbors of v
        vector<int> new_R = R;
        new_R.push_back(v);

        vector<int> new_P, new_X;
        for (int u : P)
        {
            if (adj_matrix[v][u]) new_P.push_back(u);
        }
        for (int u : X)
        {
            if (adj_matrix[v][u]) new_X.push_back(u);
        }

        bronKerbosch(new_R, new_P, new_X, max_clique, clique_count, find_max_only, adj_matrix, graph);

        // Move v from P to X
        P.erase(remove(P.begin(), P.end(), v), P.end());
        X.push_back(v);
    }
}
