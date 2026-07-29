#include <iostream>
#include "Graph.h"
#include "EulerAlgorithm.h"
#include "MaxCliqueAlgorithm.h"
#include "NumberOfCliquesAlgorithm.h"
#include "MSTWeightAlgorithm.h"
#include "HamiltonCircuitAlgorithm.h"

int main()
{
    cout << "--- 1. Testing Eulerian Circuit ---" << endl;
    Graph g_euler(5);
    g_euler.addEdge(0, 1);
    g_euler.addEdge(1, 2);
    g_euler.addEdge(2, 3);
    g_euler.addEdge(3, 4);
    g_euler.addEdge(4, 0); // Star cycle pattern
    EulerAlgorithm eulerAlgorithm(g_euler);
    string eulerAlgorithmRes = eulerAlgorithm.activate();
    cout << eulerAlgorithmRes << endl;

    cout << "\n--- 2 & 3. Testing Max Clique and Clique Counting ---" << endl;
    Graph g_clique(5);
    // Create a complete sub-graph (clique) of size 4 between nodes 0, 1, 2, 3
    g_clique.addEdge(0, 1); g_clique.addEdge(0, 2); g_clique.addEdge(0, 3);
    g_clique.addEdge(1, 2); g_clique.addEdge(1, 3);
    g_clique.addEdge(2, 3);
    g_clique.addEdge(3, 4); // Attach node 4 externally

    MaxCliqueAlgorithm maxCliqueAlgorithm(g_clique);
    string maxCliqueAlgorithmRes = maxCliqueAlgorithm.activate();
    cout << maxCliqueAlgorithmRes << endl;

    NumberOfCliquesAlgorithm numberOfCliquesAlgorithm(g_clique);
    string numberOfCliquesAlgorithmRes = numberOfCliquesAlgorithm.activate();
    cout << numberOfCliquesAlgorithmRes << endl;

    cout << "\n--- 4. Testing Minimum Spanning Tree (MST) Weight ---" << endl;
    Graph g_mst(4);
    g_mst.addEdge(0, 1, 10);
    g_mst.addEdge(0, 2, 6);
    g_mst.addEdge(0, 3, 5);
    g_mst.addEdge(1, 3, 15);
    g_mst.addEdge(2, 3, 4);
    MSTWeightAlgorithm mstWeightAlgorithm(g_mst);
    string mstWeightAlgorithmRes = mstWeightAlgorithm.activate();
    cout << mstWeightAlgorithmRes << endl;

    cout << "\n--- 5. Testing Hamiltonian Circuit ---" << endl;
    Graph g_hamilton(4);
    g_hamilton.addEdge(0, 1);
    g_hamilton.addEdge(1, 2);
    g_hamilton.addEdge(2, 3);
    g_hamilton.addEdge(3, 0); // Creates a clear 4-node ring matrix
    HamiltonCircuitAlgorithm hamiltonCircuitAlgorithm(g_hamilton);
    string hamiltonCircuitAlgorithmRes = hamiltonCircuitAlgorithm.activate();
    cout << hamiltonCircuitAlgorithmRes << endl;

    return 0;
}
