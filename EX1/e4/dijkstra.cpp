#include <iostream>
#include <vector>
#include <climits>
#include <sstream>

using namespace std;

// Function to find the vertex with the minimum distance value
// from the set of vertices not yet included in shortest path tree
int minDistance(const vector<int>& dist, const vector<bool>& sptSet)
{
    int min = INT_MAX;      // Initialize minimum value
    int min_index = -1;     // Index of minimum value

    // Loop through all vertices
    for (size_t v = 0; v < dist.size(); v++)
    {
        // If vertex is not yet processed and has smaller distance
        if (!sptSet[v] && dist[v] <= min)
        {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index; // Return index of minimum distance vertex
}

// Function to print the calculated shortest distances
void printSolution(const vector<int>& dist)
{
    cout << "Vertex\tDistance from Source" << endl;

    // Print distance of each vertex from source
    for (size_t i = 0; i < dist.size(); i++)
        cout << i << "\t\t" << dist[i] << endl;
}
/* The dijkstra is from the site geeksforgeeks with help of chat GPT */
// Dijkstra's algorithm implementation
void dijkstra(const vector<vector<int>>& graph, int src)
{
    int n = graph.size();                 // Number of vertices
    vector<int> dist(n, INT_MAX);         // Distance array (initialized to infinity)
    vector<bool> sptSet(n, false);        // Shortest Path Tree set (processed vertices)

    dist[src] = 0;                       // Distance from source to itself is 0

    // Main loop - run n-1 times
    for (int count = 0; count < n - 1; count++)
    {
        // Pick the minimum distance vertex not yet processed
        int u = minDistance(dist, sptSet);
        if (u == -1) break; // Stop if no reachable vertex is found

        sptSet[u] = true;   // Mark the picked vertex as processed

        // Update distance value of adjacent vertices
        for (int v = 0; v < n; v++)
        {
            // Update dist[v] only if:
            // 1. v is not processed
            // 2. There is an edge from u to v (non-zero weight)
            // 3. Current distance to u is not infinity
            // 4. New calculated distance is smaller than current dist[v]
            if (!sptSet[v] &&
                graph[u][v] != 0 &&
                dist[u] != INT_MAX &&
                dist[u] + graph[u][v] < dist[v])
            {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    // Print final shortest distances
    printSolution(dist);
}

int main()
{
    size_t num_of_vers;

    cout << "Enter number of vertices: " << endl;
    cin >> num_of_vers;

    // Clear leftover newline after reading the number
    cin.ignore();

    // Create adjacency matrix
    vector<vector<int>> graph(num_of_vers, vector<int>(num_of_vers));

    for (size_t i = 0; i < num_of_vers; i++)
    {
        string line;
        // Read entire line from user
        getline(cin, line);
        // Convert line into stream
        stringstream ss(line);    
        vector<int> row;
        int value;

        // Extract integers from the line
        while (ss >> value)
        {
            row.push_back(value);
        }
        if (row.size() > num_of_vers)
        {
            cout << "ERROR! too many distances\n";
            return 1;
        }
        if (row.size() < num_of_vers)
        {
            cout << "ERROR! too few distances\n";
            return 1;
        }

        for (size_t j = 0; j < num_of_vers; j++)
        {
            if (row[j] < 0)
            {
                cout << "ERROR! Dijkstra does not support negative weights!\n";
                return 1;
            }
            if (i == j && row[j] != 0)
            {
                cout << "ERROR! distance from vertex to itself must be 0\n";
                return 1;
            }

            graph[i][j] = row[j];
        }
    }
    dijkstra(graph, 0);
    return 0;
}
