#include "Graph.h"

Graph::Graph(int vertices) : n(vertices) {}

void Graph::addEdge(int u, int v, int weight)
{
    edges.push_back({ u, v, weight });
}

int Graph::getSize() const
{
    return n;
}

const vector<Edge>& Graph::getEdges() const
{
    return edges;
}

void Graph::clean(int vertices)
{
    n = vertices;
    edges.clear();
}

ostream& operator<<(ostream& s, const Graph& graph)
{
    s << "Graph with " << graph.getSize() << " vertices" << endl;
    const vector<Edge>& edges = graph.getEdges();
    int i = 1;
    for (const auto& edge : edges)
    {
        s << i++ << ". Edge from " << edge.from << " to " << edge.to;
        s << " with weight " << edge.weight << endl;
    }
    return s;
}
