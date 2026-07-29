#include <iostream>
#include "Graph.h"
#include "EulerAlgorithm.h"
#include <getopt.h>

int main(int argc, char* argv[])
{
    int c;
    int seed, vertices, edges;
    while ((c = getopt (argc, argv, "s:v:e:")) != -1)
    {
        if (c == 's')
        {
            seed = atoi(optarg);
        }
        else if (c == 'v')
        {
            vertices = atoi(optarg);
        }
        else if (c == 'e')
        {
            edges = atoi(optarg);
        }
    }
    Graph graph(vertices);
    srand(seed);
    for (int i = 0; i < edges; i++)
    {
        graph.addEdge(rand() % vertices, rand() % vertices);
    }
    cout << endl << graph << endl;
    EulerAlgorithm eulerAlgorithm;
    string eulerAlgorithmRes = eulerAlgorithm.activate(graph);
    cout << eulerAlgorithmRes << endl;
    return 0;
}

