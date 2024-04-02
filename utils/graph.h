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

struct Vertex {
public:
    int index;
    vector<Edge> edges;
    friend ostream& operator<<(ostream& os, const Vertex& v) {
        os << "Vertex index - " << v.index << ", Number of edges - " << v.edges.size();
        return os;
    }
};

struct Edge {
public:
    Vertex* first;     // Pointer to the destination vertex
    Vertex* second;   // Pointer to the source vertex
    int weight;
};

class Graph {
public:
    int V; // No. of vertices in graph
    vector<Edge> edges; // Edges in the graph
    vector<Vertex> vertices;
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
                Vertex* vertexFirst = findOrCreateVertex(firstIndex);
                Vertex* vertexSecond = findOrCreateVertex(secondIndex);
                // Only add an edge if that pairing doesnt already have one
                // This is to convert directed graphs to undirected
                cout << &vertexFirst << " : " << &vertexSecond << endl;
                if(!edgeExists(*vertexFirst, *vertexSecond)){
                    edges.push_back({vertexFirst, vertexSecond, weight});
                    addEdgeToVertex(vertexFirst, vertexSecond, edges.back());
                }
            } else {
                cerr << "Error parsing line: " << line << endl;
            }
        }

        file.close();
    }
private:

    // This is to see if we need to make a new vertex or not when created edges
    Vertex* findOrCreateVertex(int index){
        if(!vertices.empty()){
            for(const Vertex& v : vertices){
                if(v.index == index){
                    return &const_cast<Vertex&>(v);
                }
            }
        }
        // Need to make a new vertex
        vertices.push_back({index, {}});
        return &vertices.back();
    }

    // Fill the individual vertex arrays as they need to keep track of neighbours
    void addEdgeToVertex(Vertex* v1, Vertex* v2, Edge e){
        v1->edges.push_back(e);
        v2->edges.push_back(e);
    }

    // Check for duplicates
    bool edgeExists(Vertex& v1, Vertex& v2){
        cout << v1 << " : " << v2 << endl;
        if(v1.edges.empty()){
            return true;
        }
        for (const Edge& edge : v1.edges) {
            if ((edge.first == &v1 && edge.second == &v2) ||
                (edge.first == &v2 && edge.second == &v1)) {
                return true;
            }
        }
        cout << "Set Free" << endl;
        return false;
    }
};