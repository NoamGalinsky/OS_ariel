#include "EulerAlgorithm.h"
#include "Graph.h"
#include <sstream>
#include <algorithm>

EulerAlgorithm::EulerAlgorithm()
{
}

string EulerAlgorithm::activate(const Graph& graph)
{
    vector<int> euler_circ = findEulerEircle(graph);
    string str;
    if (!euler_circ.empty())
    {
        stringstream ss;
        for (int node : euler_circ)
        {
            ss << node << " ";
        }
        str = ss.str();
    }
    else
    {
        str = "No Euler Circle exists.";
    }
    return str;
}

vector<int> EulerAlgorithm::findEulerEircle(const Graph& graph)
{
    vector<vector<int>> adj_list(graph.getSize());   // Adjacency List
    const vector<Edge>& edges = graph.getEdges();
    for (const auto& edge : edges)
    {
        adj_list[edge.from].push_back(edge.to);
        adj_list[edge.to].push_back(edge.from);
    }

    // For undirected graphs, every vertex must have an even degree
    if (!hasEulerCircle(adj_list, graph))
    {
        return {};
    }

    vector<int> circuit;
    stack<int> curr_path;
    curr_path.push(0);
    int curr_v = 0;

    while (!curr_path.empty())
    {
        if (!adj_list[curr_v].empty())
        {
            curr_path.push(curr_v);
            int next_v = adj_list[curr_v].back();
            adj_list[curr_v].pop_back();

            // Remove inverse edge since graph is undirected
            adj_list[next_v].erase(remove(adj_list[next_v].begin(), adj_list[next_v].end(), curr_v), adj_list[next_v].end());

            curr_v = next_v;
        }
        else
        {
            circuit.push_back(curr_v);
            curr_v = curr_path.top();
            curr_path.pop();
        }
    }
    reverse(circuit.begin(), circuit.end());
    return circuit;
}

bool EulerAlgorithm::hasEulerCircle(vector<vector<int>>& adj_list, const Graph& graph)
{
    // For undirected graphs, every vertex must have an even degree
    int n = graph.getSize();
    for (int i = 0; i < n; ++i)
    {
        if ((adj_list[i].size() == 0) ||
            (adj_list[i].size() % 2 != 0))
        {
            return false;
        }
    }
    return true;
}
