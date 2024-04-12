#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <queue>
#include <iterator>
#include <cstdlib>
#include <ctime>
#include <stdexcept>

using namespace std;

class Edge;

class Vertex {
public:
    int id;
    std::vector<Edge*> edges;
    int color = -1;
    Vertex(int id) : id(id) {}
    
    void addEdge(Edge* edge) {
        edges.push_back(edge);
    }
    
};

class Edge {
public:
    Vertex* vertex1;
    Vertex* vertex2;
    int weight;
    
    Edge(Vertex* v1, Vertex* v2, int w) : vertex1(v1), vertex2(v2), weight(w) {}
    bool operator<(const Edge& other) const {
        // Compare edges based on their weights
        return weight < other.weight;
    }

    bool operator==(const Edge& other) const {
        // Compare edges based on their weights
        return ((vertex1->id == other.vertex1->id && vertex2->id == other.vertex2->id)
                || (vertex1->id == other.vertex2->id && vertex2->id == other.vertex1->id));
    }
};

// For checking if an edge is a duplicate
struct EdgeHasher {
    std::size_t operator()(const Edge& edge) const noexcept {
        int low = std::min(edge.vertex1->id, edge.vertex2->id);
        int high = std::max(edge.vertex1->id, edge.vertex2->id);
        // Create a hash for the mix of low high. Unordered so each edge exists once
        return std::hash<int>()(low) ^ std::hash<int>()(high);
    }
};


class Graph {
public:
    std::unordered_map<int, Vertex*> vertices;
    int V;

    Graph(){
        std::srand(std::time(nullptr));
    }

    void addEdge(int vertexId1, int vertexId2, int weight) {
        Vertex* v1 = getOrCreateVertex(vertexId1);
        Vertex* v2 = getOrCreateVertex(vertexId2);
        Edge* edge = new Edge(v1, v2, weight);
        v1->addEdge(edge);
        v2->addEdge(edge);
    }
    
    Vertex* getOrCreateVertex(int id) {
        if (vertices.find(id) == vertices.end()) {
            vertices[id] = new Vertex(id);
        }
        return vertices[id];
    }
    
    void readCSV(const std::string& filePath) {
        std::ifstream file(filePath);
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            int vertexId1, vertexId2;
            int weight;
            string temp;
            std::getline(ss, temp, ',');
            vertexId1 = stoi(temp);

            std::getline(ss, temp, ',');
            vertexId2 = stoi(temp);

            ss >> weight;
            addEdge(vertexId1, vertexId2, weight);
        }
        V = vertices.size(); // init the size
    }

    // Function to perform Depth-First Search (DFS)
    void DFS(Vertex* vertex, std::unordered_set<int>& visited) {
        visited.insert(vertex->id); 
        // Traverse all connected vertices
        for (Edge* edge : vertex->edges) {
            Vertex* adjacentVertex = (edge->vertex1 == vertex) ? edge->vertex2 : edge->vertex1;
            // if vertex is not visited, recursively call DFS
            if (visited.find(adjacentVertex->id) == visited.end()) {
                DFS(adjacentVertex, visited);
            }
        }
    }
    // Check if we are connected 
    bool isConnected() {
        if (vertices.empty()) // If the graph has no vertices, it is trivially connected
            return true;

        std::unordered_set<int> visited;
        // Start DFS from any vertex
        Vertex* startVertex = vertices.begin()->second; // Choose any vertex as starting point
        DFS(startVertex, visited);
        // If number of visited vertices equals total number of vertices, the graph is connected
        return visited.size() == vertices.size();
    }

    Vertex* getRandomUnvisitedVertex(){
        int color = 0;
        Vertex* random_item;
        if(vertices.empty()){
            throw std::invalid_argument( "Empty graph" );
            return nullptr;
        }
        // if no more empty then return null
        std::vector<Vertex*> vertexVector;
        for (const auto& pair : vertices) {
            if(pair.second->color == -1){
                vertexVector.push_back(pair.second);
            }
        }
        if(vertexVector.size() == 0){
            cout << "No more unvisited vertices" << endl;
            return nullptr;
        }
        int randomIndex = std::rand() % vertexVector.size();
        while(color != -1){
            Vertex* randomVertex = vertexVector[randomIndex];
            color = randomVertex->color;
            randomIndex = std::rand() % vertexVector.size();
        }
        random_item = vertices[randomIndex];
        return random_item;
    }
};
