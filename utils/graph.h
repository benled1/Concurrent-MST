#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>

/*
Edges
- 2 Vertices
- Weight
Vertex
- Holds a list of edges
- ID to identify 
Graph
- Adjacency List?
- Root node?
*/

using namespace std;

struct Edge;

class Vertex {
public:
    vector<Edge> edges;
};

struct Edge {
    Vertex* to;     // Pointer to the destination vertex
    Vertex* from;   // Pointer to the source vertex
    int weight;
};

class Graph {
public:
    int V; // No. of vertices in graph
    vector<vector<int>> adjMatrix;
    vector<Edge> edges; // Edges in the graph

    // Create a graph from an input file
    Graph(string filename){

    }
};