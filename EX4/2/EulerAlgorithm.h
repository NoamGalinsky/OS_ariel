#pragma once

#include "GraphAlgorithm.h"

#include <vector>
#include <stack>

using namespace std;

// class for activating the Euler algorithm
class EulerAlgorithm :
    public GraphAlgorithm
{
public:
	EulerAlgorithm();
	virtual string activate(const Graph& graph);

private:
    // Find Euler Circle (Eulerian Circuit) using Hierholzer's Algorithm
    vector<int> findEulerEircle(const Graph& graph);
    // check if the graph can have Euler circle
    bool hasEulerCircle(vector<vector<int>>& adj_list, const Graph& graph);
};
