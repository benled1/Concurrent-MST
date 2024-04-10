#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <numeric>
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
    vector<Edge> local_min_edges;
    vector<Edge> global_min_edges;
    bool edges_remaining = true;
    
    
    // get portion of the ds that this process is responsible for
    vector<vector<int>> ds_range;
    int min_vertices_per_process = ds.size / world_size;
    int remaining_vertices = ds.size % world_size;
    int start_idx = 0;

    for (int i=0;i<world_size;i++) {
        int vertices_for_curr = min_vertices_per_process + (i < remaining_vertices ? 1 : 0);
        int end_idx = start_idx + vertices_for_curr;
        ds_range.push_back({start_idx, end_idx});
        start_idx = end_idx;
    }

    // print out the ds_range for debugging purposes
    if (world_rank==0) {
        for (int i=0;i<world_size;i++) {
            cout<<"Process "<<i<<","<<"{"<<ds_range[i][0]<<", "<<ds_range[i][1]<<"}"<<endl;
        }
    }

    // in parallel loop over portion that was allocated
    for(int i=ds_range[world_rank][0]; i<ds_range[world_rank][1];i++) {
        if(ds.find(i)!=i) {
            continue;
        }
        vector<int> connected_vertices = ds.getConnectedIds(i);
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
        if (min_leaving_edge==nullptr){
            continue;
        }
        local_min_edges.push_back(*min_leaving_edge);
    }
    // at this point we should have an array of min outgoing edges from each of the 
    // subgraphs in the partition of ds for this process

    // build three arrays which can be sent over MPI 
    int local_weights[local_min_edges.size()];
    int local_vertex1s[local_min_edges.size()];
    int local_vertex2s[local_min_edges.size()];
    for (int i=0;i<local_min_edges.size();i++) {
        local_weights[i] = local_min_edges[i].weight;
        local_vertex1s[i] = local_min_edges[i].vertex1->id;
        local_vertex2s[i] = local_min_edges[i].vertex2->id;
    }

    // prepate for gathering
    int local_count = local_min_edges.size();
    int* counts = nullptr; // counts of elements sent by each process
    int* displacements = nullptr; // displacements where each segment begins

    if (world_rank==0) {
        counts = new int[world_size];
        displacements = new int[world_size];
    }

    // gather the size of array being sent to process 0
    MPI_Gather(&local_count, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank==0) {
        displacements[0] = 0;
        for (int i=1;i<world_size;i++) {
            displacements[i] = displacements[i-1] + counts[i-1];
        }
    }

    int total_count;
    if (world_rank==0) {
        total_count = std::accumulate(counts, counts+world_size, 0);
    } else {
        total_count = 0;
    }

    int* all_weights;
    int* all_vertex1s;
    int* all_vertex2s;
    if (world_rank==0) {
        all_weights = new int[total_count];
        all_vertex1s = new int[total_count];
        all_vertex2s = new int[total_count];
    } else {
        all_weights = nullptr;
        all_vertex1s = nullptr;
        all_vertex2s = nullptr;    
    }

    MPI_Gatherv(local_weights, local_count, MPI_INT,
                all_weights, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Gatherv(local_vertex1s, local_count, MPI_INT,
                all_vertex1s, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Gatherv(local_vertex2s, local_count, MPI_INT,
                all_vertex2s, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);

    if (world_rank==0) {
        for (int i=0;i<total_count;i++) {
            cout<<"Weight "<<i<<" = "<<all_weights[i]<<endl;
        }
    }

    // delete[] local_weights;
    // if (world_rank == 0) {
    //     delete[] counts;
    //     delete[] displacements;
    //     delete[] all_weights; // When done processing
    // }
    // in process 0 we will have to match the id to the vertex
    // when rebuilding the Edge object, 



    // // sort the min_edges vector which now has a min_edge for every subgraph in the subgraph_set
    // sort(min_edges.begin(), min_edges.end());


    // // loop while there are still edges in the min_edges vector
    // while(!min_edges.empty()){
    //     for (int i=0; i<min_edges.size();i++) {
    //         if(ds.find(min_edges[i].vertex1->id) != ds.find(min_edges[i].vertex2->id)) {
    //             ds.merge(min_edges[i].vertex1->id, min_edges[i].vertex2->id);
    //             mst.push_back(min_edges[i]);
    //         }
    //     }
        
    //     // clear the min_edges
    //     min_edges.clear();

    //     for(int i=0; i<ds.size;i++) {
    //         if(ds.find(i)!=i) {
    //             continue;
    //         }
    //         vector<int> connected_vertices = ds.getConnectedIds(i);
    //         Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, connected_vertices);
    //         if (min_leaving_edge==nullptr){
    //             continue;
    //         }
    //         min_edges.push_back(*min_leaving_edge);
    //     }

    //     sort(min_edges.begin(), min_edges.end());
    // }

    // return mst;
}


int main(int argc, char *argv[]){
    if (argc <= 1) {
        cout<<"mst_serial requires at least a filepath."<<endl;
        return 0;
    }
    bool testVerificationLogs = false;
    string graphFilePath = argv[1];
    if (argc >= 3) {
        testVerificationLogs = argv[2];
    }

    Graph g;

    g.readCSV(graphFilePath);
    
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