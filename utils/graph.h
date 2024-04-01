#include <vector>

/*
Edges
- 2 Vertices
- Weight
Vertex
- Holds a list of edges
- ID to identify 
Graph
- 
*/

class Vertex {
    public:
        Vertex* neighbours;
        
};

class Graph {
    public:
        int V; // No. of vertices in graph
        vector<vector<pair<int, int>>> adjList;  // Pointer to an array containing adjacency lists
        vector<vector<pair<int, int>>> adjWeights;  // Pointer to an array containing adjacency weights
};