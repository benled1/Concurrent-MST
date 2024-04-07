#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert> // For assert()
#include <algorithm>
#include <string>
#include <limits>

using namespace std;

bool isLeavingEdge(Edge& edge, vector<int> connected_vertices) {
    if (find(connected_vertices.begin(), connected_vertices.end(), edge.vertex1->id) == connected_vertices.end() ||
        find(connected_vertices.begin(), connected_vertices.end(), edge.vertex2->id) == connected_vertices.end()) {
        return true;
    }
    return false;
}


Edge* findMinOutGoingEdge(Graph& graph, vector<int> connected_vertices) {

    Edge* minEdge = nullptr;
    int minWeight = numeric_limits<int>::max();

    for(int vertexId: connected_vertices){
        Vertex* currVertex = graph.vertices[vertexId];
        for(Edge* edge: currVertex->edges) {
            // check if the edge is leaving the connected set
            if (isLeavingEdge(*edge, connected_vertices)) {
                if (edge->weight < minWeight) {
                    minWeight = edge->weight;
                    minEdge = edge;
                }
            } else {
                continue;
            }
        }
    }
    return minEdge;
}


// below is the serial version of the distributed program (does everything on process 0)
vector<Edge> distributedPrims(Graph& inputGraph) {
    // init the disjoint set, final mst, and min_edges
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> min_edges;

    for(int i=0; i<ds.size;i++) {
        if(ds.find(i)!=i) {
            continue;
        }
        vector<int> connected_vertices = ds.getConnectedIds(i);
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
        if (min_leaving_edge==nullptr){
            continue;
        }
        min_edges.push_back(*min_leaving_edge);
    }

    // sort the min_edges vector which now has a min_edge for every subgraph in the subgraph_set
    sort(min_edges.begin(), min_edges.end());


    // loop while there are still edges in the min_edges vector
    while(!min_edges.empty()){
        for (int i=0; i<min_edges.size();i++) {
            if(ds.find(min_edges[i].vertex1->id) != ds.find(min_edges[i].vertex2->id)) {
                ds.merge(min_edges[i].vertex1->id, min_edges[i].vertex2->id);
                mst.push_back(min_edges[i]);
            }
        }
        
        // clear the min_edges
        min_edges.clear();

        for(int i=0; i<ds.size;i++) {
            if(ds.find(i)!=i) {
                continue;
            }
            vector<int> connected_vertices = ds.getConnectedIds(i);
            Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
            if (min_leaving_edge==nullptr){
                continue;
            }
            min_edges.push_back(*min_leaving_edge);
        }

        sort(min_edges.begin(), min_edges.end());

        for (Edge edge: min_edges) {
            cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
        }
    }

    return mst;
}


int main(int argc, char *argv[]){
    Graph g;
    g.readCSV("./inputGraphs/testData.csv");
    for (auto& vertexPair : g.vertices) {
        std::cout << "Vertex: " << vertexPair.first << std::endl;
        for (Edge* edge : vertexPair.second->edges) {
            std::cout << "  connects to: " << edge->vertex1->id << " - " << edge->vertex2->id;
            std::cout << " with weight: " << edge->weight << std::endl;
        }
    }

    vector<Edge> mst = distributedPrims(g);

    cout<<"MST:"<<endl;
    for(Edge edge: mst) {
        cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
    }
}