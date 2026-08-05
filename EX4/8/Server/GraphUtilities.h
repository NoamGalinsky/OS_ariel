#pragma once

#include <vector>

using namespace std;

class Graph;

// class for general graph utilities
class GraphUtilities
{
public:
    // Fill adjacency matrix
	static void fillAdjMatrix(vector<vector<int>>& adj_matrix, const Graph& graph);

    // Helper for Bron-Kerbosch Algorithm (Cliques)
    static void bronKerbosch(vector<int>& R, vector<int>& P, vector<int>& X,
        vector<int>& max_clique, int& clique_count, bool find_max_only,
        vector<vector<int>>& adj_matrix, const Graph& graph);
};
