#include "GraphAlgorithmFactory.h"
#include "EulerAlgorithm.h"
#include "HamiltonCircuitAlgorithm.h"
#include "MaxCliqueAlgorithm.h"
#include "MSTWeightAlgorithm.h"
#include "NumberOfCliquesAlgorithm.h"

GraphAlgorithmFactory* GraphAlgorithmFactory::getGraphAlgorithmFactory()
{
    // create a static instance
    static GraphAlgorithmFactory graphAlgorithmFactory;
    // return the address of the instance
    return &graphAlgorithmFactory;
}

GraphAlgorithm* GraphAlgorithmFactory::getGraphAlgorithm(const string& name)
{
    auto algorithm = graphAlgorithms.find(name);
    if (algorithm != graphAlgorithms.end())
    {
        return algorithm->second;
    }
    return nullptr;
}

GraphAlgorithmFactory::GraphAlgorithmFactory()
{
    // create the algorithms and fill the map
    static EulerAlgorithm eulerAlgorithm;
    graphAlgorithms["euler"] = &eulerAlgorithm;
    static HamiltonCircuitAlgorithm hamiltonCircuitAlgorithm;
    graphAlgorithms["hamilton"] = &hamiltonCircuitAlgorithm;
    static MaxCliqueAlgorithm maxCliqueAlgorithm;
    graphAlgorithms["max"] = &maxCliqueAlgorithm;
    static MSTWeightAlgorithm mstWeightAlgorithm;
    graphAlgorithms["mst"] = &mstWeightAlgorithm;
    static NumberOfCliquesAlgorithm numberOfCliquesAlgorithm;
    graphAlgorithms["num"] = &numberOfCliquesAlgorithm;
}
