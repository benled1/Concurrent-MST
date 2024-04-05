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

struct Edge;

struct Vertex {
    int id;
    vector<Edge> edges_connnected_to;
    int color = -1;

    Vertex(int index){
        id = index;
    }
};  

struct Edge {
public:
    Vertex first;  // First index
    Vertex second; // Second index
    int weight;

    bool operator<(const Edge& other) const {
        return weight > other.weight;
    }
};

class Graph {
public:
    int V; // No. of vertices in graph
    vector<Edge> edges; // Edges in the graph
    vector<Vertex> vertices; // Vertices in the graph
    int maxV = 0;
    int minV = 0;
    // Create a graph from an input file
    Graph(string filename){
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
        }

        string line;
        bool first = true;
        while (getline(file, line)) {
            istringstream iss(line);
            int firstIndex, secondIndex, weight;
            char comma;
            if (iss >> firstIndex >> comma >> secondIndex >> comma >> weight) {
                // Only add an edge if that pairing doesnt already have one
                // This is to convert directed graphs to undirected
                Vertex& firstVertex = findOrCreateVertex(firstIndex);
                Vertex& secondVertex = findOrCreateVertex(secondIndex);
                if(!edgeExists(firstIndex, secondIndex)){
                    Edge newEdge = {firstVertex,secondVertex,weight};
                    edges.push_back(newEdge);
                    // Add the edge to the vertices
                    firstVertex.edges_connnected_to.push_back(newEdge);
                    secondVertex.edges_connnected_to.push_back(newEdge);
                }
                // Update max and min
                if(first == true){
                    maxV = max({firstVertex.id, secondVertex.id});
                    minV = min({firstVertex.id, secondVertex.id});
                    first = false;
                }
                else{
                    maxV = max({maxV, firstVertex.id, secondVertex.id});
                    minV = min({minV, firstVertex.id, secondVertex.id});
                }
            } else {
                cerr << "Error parsing line: " << line << endl;
            }
        }
        file.close();
        V = maxV - minV + 1;
    }

    Vertex& findOrCreateVertex(int index) {
        for(auto& vertex : vertices){
            if(vertex.id == index){
                return vertex;
            }
        }
        Vertex newVertex(index);
        vertices.push_back(newVertex);
        return vertices.back();
    }

    bool hasDuplicateEdges() {
        set<Edge> edgeSet;
        for (const auto& edge : edges) {
            Edge sortedEdge = {min(edge.first.id, edge.second.id), max(edge.first.id, edge.second.id), edge.weight};

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
            if((e.first.id == v1 && e.second.id == v2) || (e.first.id == v2 && e.second.id == v1)){
                return true;
            }
        }
        return false;
    }

    Vertex& getRandomUnvisitedVertex(){
        int randomIndex = -1;
        while(randomIndex == -1){
            random_device rd; // obtain a random number from hardware
            mt19937 gen(rd()); // seed the generator
            uniform_int_distribution<> distr(minV, maxV); // define the range
            randomIndex = distr(gen);
            for(auto& vertex : vertices){
                if(vertex.id == randomIndex){
                    return vertex;
                }
            }
            randomIndex = -1;
        }
    }

};