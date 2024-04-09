#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <limits>
#include </usr/include/mpi/mpi.h>

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


// below is the distributed version of prims 
vector<Edge> distributedPrims(Graph& inputGraph, int world_size, int world_rank) {
    // init the disjoint set, final mst, and min_edges
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> min_edges;
    bool edges_remaining = true;
    
    
    // get portion of the ds that this process is responsible for
    vector<int> ds_range;
    int min_vertices_per_process = ds.size / world_size;
    int remaining_vertices = ds.size % world_size;

    if (world_rank==0) {
        ds_range = {0, min_vertices_per_process};
        if (world_rank < remaining_vertices) {
            ds_range[1]++;
            remaining_vertices--;
        }
    }

    




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
    }

    return mst;
}


int main(int argc, char *argv[]){
    Graph g;
    g.readCSV("./inputGraphs/connected_graph.csv");
    
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;

    MPI_Get_processor_name(processor_name, &name_len);

    vector<Edge> mst = distributedPrims(g, world_size, world_rank);

    if (world_rank==0) {
        cout<<"MST:"<<endl;
        for(Edge edge: mst) {
            cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
        }
    }
    MPI_Finalize();
    return 0;
}