#pragma once
#include "GraphAlgorithm.h"

// class for activating the Number of cliques algorithm
class NumberOfCliquesAlgorithm :
    public GraphAlgorithm
{
public:
    NumberOfCliquesAlgorithm();
    virtual string activate(const Graph& graph);

private:
    // Finding Total Number of Cliques using Bron-Kerbosch
    int findNumberOfCliques(const Graph& graph);
};
