#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>

using namespace std;

class Vertex;

class Edge {
public:
    Vertex* vertex1;
    Vertex* vertex2;
    int weight;
    
    Edge(Vertex* v1, Vertex* v2, int w) : vertex1(v1), vertex2(v2), weight(w) {}

    bool operator<(const Edge& other) const {
        return this->weight < other.weight;
    }
};

class Vertex {
public:
    int id;
    std::vector<Edge*> edges;
    
    Vertex(int id) : id(id) {}
    
    void addEdge(Edge* edge) {
        edges.push_back(edge);
    }
};


class Graph {
public:
    std::unordered_map<int, Vertex*> vertices;
    int V;
    
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
};
