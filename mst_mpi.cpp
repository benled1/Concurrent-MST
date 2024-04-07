#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert> // For assert()
#include <algorithm>
#include <string>
#include <limits>

using namespace std;

// // implement a disjoint set data structure here
// // https://www.geeksforgeeks.org/disjoint-set-data-structures/

// // TODO:
// //     implement a DisjointSet look at the link above
// //     implement the findMinOutGoingEdge
// //     implement the full algorithm but in a serial fashion like written below
// //     add the distributed portions by partitioning the edge set and adding broadcasting

bool isLeavingEdge(Edge& edge, vector<int> connected_vertices) {
    if (find(connected_vertices.begin(), connected_vertices.end(), edge.vertex1->id) == connected_vertices.end() ||
        find(connected_vertices.begin(), connected_vertices.end(), edge.vertex2->id) == connected_vertices.end()) {
        return true;
    }
    return false;
}


Edge* findMinOutGoingEdge(Graph& graph, vector<int> connected_vertices) {

    Edge* minEdge;
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
void distributedPrims(Graph& inputGraph) {
    // init the disjoint set, final mst, and min_edges
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> min_edges;

    // find the minimum outgoing edge for each subgraph (this iteration, each subgraph is a vertex)
    // if there is a minimum outgoing edge, push it onto the min_edges vector
    for(int i=0; i<ds.size;i++) {
        vector<int> connected_vertices = ds.getConnectedIds(i);
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
        min_edges.push_back(*min_leaving_edge);
    }

    // sort the min_edges vector which now has a min_edge for every subgraph in the subgraph_set
    sort(min_edges.begin(), min_edges.end());

    for (Edge edge: min_edges) {
        cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
    }




    // loop while there are still edges in the min_edges vector
    while(!min_edges.empty()){
        // for every min edge, check if it connects two disjoint subgraphs, if yes, merge the two it connects
        // if a min edge merges two subgraphs, push it to our final mst.
        for (int i; i<min_edges.size();i++) {
            if(ds.find(min_edges[i].vertex1->id) != ds.find(min_edges[i].vertex2->id)) {
                ds.merge(min_edges[i].vertex1->id, min_edges[i].vertex2->id);
                mst.push_back(min_edges[i]);
            }
        }
        
        // clear the min_edges
        min_edges.clear();

        // refill the min_edges with the new min_leaving edges on the new merged subgraphs
        // there should be a min edge for every new subgraph in the subgraphs_set
        for(int i=0; i<ds.size;i++) {
            vector<int> connected_vertices = ds.getConnectedIds(i);
            Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
            min_edges.push_back(*min_leaving_edge);
        }


        sort(min_edges.begin(), min_edges.end());
        for (Edge edge: min_edges) {
            cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
        }
        cout<<"REPETITION END"<<endl;
        // repeat above until we find no min outgoing edges due to the subgraphs_set only having one subgraph.
    }

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
    distributedPrims(g);
    
    // return 0;

    // testConstructor();
    // testMerge();
    // testFind();
    // testGetConnectedIds();
    
}