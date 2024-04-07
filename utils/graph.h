#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <queue>

class Vertex;

class Edge {
public:
    Vertex* vertex1;
    Vertex* vertex2;
    int weight;
    
    Edge(Vertex* v1, Vertex* v2, int w) : vertex1(v1), vertex2(v2), weight(w) {}
};

class Vertex {
public:
    std::string id;
    std::vector<Edge*> edges;
    
    Vertex(std::string id) : id(id) {}
    
    void addEdge(Edge* edge) {
        edges.push_back(edge);
    }
};


class Graph {
public:
    std::unordered_map<std::string, Vertex*> vertices;
    int V;
    
    void addEdge(std::string vertexId1, std::string vertexId2, int weight) {
        Vertex* v1 = getOrCreateVertex(vertexId1);
        Vertex* v2 = getOrCreateVertex(vertexId2);
        Edge* edge = new Edge(v1, v2, weight);
        v1->addEdge(edge);
        v2->addEdge(edge);
    }
    
    Vertex* getOrCreateVertex(std::string id) {
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
            std::string vertexId1, vertexId2;
            int weight;
            std::getline(ss, vertexId1, ',');
            std::getline(ss, vertexId2, ',');
            ss >> weight;
            addEdge(vertexId1, vertexId2, weight);
        }
        V = vertices.size(); // init the size
    }

    // Function to perform Depth-First Search (DFS)
    void DFS(Vertex* vertex, std::unordered_set<std::string>& visited) {
        visited.insert(vertex->id); // Mark current vertex as visited
        // Traverse all adjacent vertices
        for (Edge* edge : vertex->edges) {
            Vertex* adjacentVertex = (edge->vertex1 == vertex) ? edge->vertex2 : edge->vertex1;
            // If adjacent vertex is not visited, recursively call DFS
            if (visited.find(adjacentVertex->id) == visited.end()) {
                DFS(adjacentVertex, visited);
            }
        }
    }

    // Function to check if the graph is connected
    bool isConnected() {
        if (vertices.empty()) // If the graph has no vertices, it is trivially connected
            return true;

        std::unordered_set<std::string> visited;
        // Start DFS from any vertex
        Vertex* startVertex = vertices.begin()->second; // Choose any vertex as starting point
        DFS(startVertex, visited);
        // If number of visited vertices equals total number of vertices, the graph is connected
        return visited.size() == vertices.size();
    }
};
