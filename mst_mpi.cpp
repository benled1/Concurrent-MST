#include "utils/graph.h"
#include <mpi.h>
using namespace std;

// implement a disjoint set data structure here
// https://www.geeksforgeeks.org/disjoint-set-data-structures/


class DisjointSet {
public:
    DisjointSet(int v) {}
    bool Connects(Edge* e) {}
    void Merge(int vertex1, int vertex2) {}
    int size();
    Graph index();

};

Edge findMinOutGoingEdge(Graph* subgraph) {
    // a function to find the min outgoing edge from a vertex
    Edge result;
    return result;
}

// below is the serial version of the distributed program (does everything on process 0)
void distributedPrims(Graph* inputGraph, int process_num, int world_size) {
    // init the disjoint set, final mst, and min_edges
    DisjointSet subgraphs_set = DisjointSet(inputGraph->V);
    vector<int> mst;
    vector<Edge> min_edges;

    // find the minimum outgoing edge for each subgraph (this iteration, each subgraph is a vertex)
    // if there is a minimum outgoing edge, push it onto the min_edges vector
    for(int i; i<subgraphs_set.size();i++) {
        Edge min_leaving_edge = findMinOutGoingEdge(&subgraphs_set.index());
        if (min_leaving_edge) {
            min_edges.push_back(min_leaving_edge);
        }
    }

    // sort the min_edges vector which now has a min_edge for every subgraph in the subgraph_set
    min_edges.sort();

    // loop while there are still edges in the min_edges vector
    while(min_edges){
        // for every min edge, check if it connects two disjoint subgraphs, if yes, merge the two it connects
        // if a min edge merges two subgraphs, push it to our final mst.
        for (int i; i<min_edges.size();i++) {
            if (subgraphs_set.Connects(&min_edges[i])) {
                subgraphs_set.Merge(min_edges[i].first, min_edges[i].second);
                mst.push_back(min_edges[i]);
            }
        }
        
        // clear the min_edges
        min_edges.clear();

        // refill the min_edges with the new min_leaving edges on the new merged subgraphs
        // there should be a min edge for every new subgraph in the subgraphs_set
        for(int i; i<subgraphs_set.size();i++) {
            Edge min_leaving_edge = findMinOutGoingEdge(&subgraphs_set[i]);
            if (min_leaving_edge) {
                min_edges.push_back(min_leaving_edge);
            }
        }
        // repeat above until we find no min outgoing edges due to the subgraphs_set only having one subgraph.
    }
    
    return mst;
}


int main(int argc, char *argv[]){

}