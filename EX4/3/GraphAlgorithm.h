#pragma once

#include <string>

using namespace std;

class Graph;

// run an algorithm on a graph

class GraphAlgorithm
{
public:
	GraphAlgorithm();
	// activate an algorithm on the graph
	virtual string activate(const Graph& graph) = 0;
};
