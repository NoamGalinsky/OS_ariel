#include "MSTWeightAlgorithm.h"
#include "Graph.h"
#include <numeric>
#include <sstream>
#include <algorithm>

bool operator<(const Edge& me, const Edge& other)
{
    return me.weight < other.weight;
}

DSU::DSU(int n)
{
    parent.resize(n);
    iota(parent.begin(), parent.end(), 0);
    rank.assign(n, 0);
}

int DSU::find(int i)
{
    if (parent[i] == i)
    {
        return i;
    }
    return parent[i] = find(parent[i]);
}

bool DSU::unite(int i, int j)
{
    int root_i = find(i);
    int root_j = find(j);
    if (root_i != root_j)
    {
        if (rank[root_i] < rank[root_j])
            swap(root_i, root_j);
        parent[root_j] = root_i;
        if (rank[root_i] == rank[root_j])
            rank[root_i]++;
        return true;
    }
    return false;
}

MSTWeightAlgorithm::MSTWeightAlgorithm()
{
}

string MSTWeightAlgorithm::activate(const Graph& graph)
{
    stringstream ss;
    ss << "Total MST weight: " << findMstWeight(graph);
    return ss.str();
}

int MSTWeightAlgorithm::findMstWeight(const Graph& graph)
{
    int n = graph.getSize();
    vector<Edge> edges = graph.getEdges();
    sort(edges.begin(), edges.end());
    DSU dsu(n);
    int mst_weight = 0;
    int edges_used = 0;

    for (const auto& edge : edges)
    {
        if (dsu.unite(edge.from, edge.to))
        {
            mst_weight += edge.weight;
            edges_used++;
            if (edges_used == n - 1)
            {
                break;
            }
        }
    }
    return (edges_used == n - 1) ? mst_weight : -1; // Return -1 if graph is disconnected
}
