#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
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

struct Edge {
public:
    int first;  // First index
    int second; // Second index
    int weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class Graph {
public:
    int V; // No. of vertices in graph
    vector<Edge> edges; // Edges in the graph
    // Create a graph from an input file
    Graph(string filename){
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            int firstIndex, secondIndex, weight;
            char comma;
            if (iss >> firstIndex >> comma >> secondIndex >> comma >> weight) {
                // Only add an edge if that pairing doesnt already have one
                // This is to convert directed graphs to undirected
                if(!edgeExists(firstIndex, secondIndex)){
                    edges.push_back({firstIndex, secondIndex, weight});
                    // addEdgeToVertex(vertexFirst, vertexSecond, edges.back());
                }
            } else {
                cerr << "Error parsing line: " << line << endl;
            }
        }

        file.close();
    }

    bool hasDuplicateEdges() {
        set<Edge> edgeSet;

        for (const auto& edge : edges) {
            Edge sortedEdge = {min(edge.first, edge.second), max(edge.first, edge.second), edge.weight};

            // Check if the edge already exists in the set
            if (edgeSet.find(sortedEdge) != edgeSet.end()) {
                // Duplicate found
                return true;
            }

            // If not found, insert it into the set
            edgeSet.insert(sortedEdge);
        }

        // No duplicates found
        return false;
    }

    bool edgeExists(int v1, int v2){
        if(edges.empty()){
            return false;
        }
        for (Edge e : edges){
            if((e.first == v1 && e.second == v2) || (e.first == v2 && e.second == v1)){
                return true;
            }
        }
        return false;
    }

};