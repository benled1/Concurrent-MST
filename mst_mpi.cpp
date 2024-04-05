#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert> // For assert()
#include <algorithm>

using namespace std;

// implement a disjoint set data structure here
// https://www.geeksforgeeks.org/disjoint-set-data-structures/

//TODO:
    // implement a DisjointSet look at the link above
    // implement the findMinOutGoingEdge
    // implement the full algorithm but in a serial fashion like written below
    // add the distributed portions by partitioning the edge set and adding broadcasting


Edge findMinOutGoingEdge(Graph& graph, vector<int> vertex_indices) {
    // a function to find the min outgoing edge from a vertex
    // iterate over the vertex list in the graph and look for the minedge that 
    // does not have both ends of it in the vertex_indices
    return graph.edges[0];
}

// below is the serial version of the distributed program (does everything on process 0)
void distributedPrims(Graph& inputGraph) {
    // init the disjoint set, final mst, and min_edges
    DisjointSet dset(inputGraph.vertices);
    vector<Edge> mst;
    vector<Edge> min_edges;

    // find the minimum outgoing edge for each subgraph (this iteration, each subgraph is a vertex)
    // if there is a minimum outgoing edge, push it onto the min_edges vector
    for(int i; i<dset.size;i++) {
        Edge min_leaving_edge = findMinOutGoingEdge(inputGraph, dset.getConnectedIds(i));
        min_edges.push_back(min_leaving_edge);
    }

    // sort the min_edges vector which now has a min_edge for every subgraph in the subgraph_set
    sort(min_edges.begin(), min_edges.end());


    // loop while there are still edges in the min_edges vector
    // while(min_edges){
    //     // for every min edge, check if it connects two disjoint subgraphs, if yes, merge the two it connects
    //     // if a min edge merges two subgraphs, push it to our final mst.
    //     for (int i; i<min_edges.size();i++) {
    //         if (subgraphs_set.Connects(&min_edges[i])) {
    //             subgraphs_set.Merge(min_edges[i].first, min_edges[i].second);
    //             mst.push_back(min_edges[i]);
    //         }
    //     }
        
    //     // clear the min_edges
    //     min_edges.clear();

    //     // refill the min_edges with the new min_leaving edges on the new merged subgraphs
    //     // there should be a min edge for every new subgraph in the subgraphs_set
    //     for(int i; i<subgraphs_set.size();i++) {
    //         Edge min_leaving_edge = findMinOutGoingEdge(&subgraphs_set[i]);
    //         if (min_leaving_edge) {
    //             min_edges.push_back(min_leaving_edge);
    //         }
    //     }
    //     // repeat above until we find no min outgoing edges due to the subgraphs_set only having one subgraph.
    // }

    // return mst;
}


int main(int argc, char *argv[]){
    // Graph inputGraph = Graph("inputGraphs/testData.csv");
    // vector<Edge> edges = inputGraph.edges;
    // sort(edges.begin(), edges.end());
    // for (int i = 0; i<edges.size();i++) {
    //     cout << "Edge" << edges[i].weight << endl;
    // }
    
}