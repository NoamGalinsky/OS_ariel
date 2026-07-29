#include <iostream>
#include "Graph.h"
#include "EulerAlgorithm.h"

int main()
{
    cout << "--- Testing Eulerian Circuit ---" << endl;
    Graph g_euler(5);
    g_euler.addEdge(0, 1);
    g_euler.addEdge(1, 2);
    g_euler.addEdge(2, 3);
    g_euler.addEdge(3, 4);
    g_euler.addEdge(4, 0); // Star cycle pattern
    EulerAlgorithm eulerAlgorithm;
    string eulerAlgorithmRes = eulerAlgorithm.activate(g_euler);
    cout << eulerAlgorithmRes << endl;

    g_euler.clean(5);
    g_euler.addEdge(0, 1);
    g_euler.addEdge(1, 2);
    g_euler.addEdge(2, 3);
    g_euler.addEdge(3, 4);
    eulerAlgorithmRes = eulerAlgorithm.activate(g_euler);
    cout << eulerAlgorithmRes << endl;
    return 0;
}
