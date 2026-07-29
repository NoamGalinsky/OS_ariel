#pragma once

#include <vector>
#include <iostream>

using namespace std;

// Edge structure
struct Edge
{
    // from vertice
    int from;
    // to vertice
    int to;
    // vertice weight
    int weight;
};

// class for graph data structure
class Graph
{
public:
    Graph(int vertices);

    // Add Edge function supporting both weighted and unweighted structures
    void addEdge(int u, int v, int weight = 1);

    int getSize() const;

    const vector<Edge>& getEdges() const;

    // set the number of vertices and remove all edges (reset to a new size)
    void clean(int vertices);

private:
    int n; // Number of vertices

    vector<Edge> edges;
};

// write the graph into a stream
ostream& operator<<(ostream& s, const Graph& graph);
