#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <numeric>
#include <limits>
#include </usr/include/mpi/mpi.h>
#include <ctime>
#include <chrono>

using namespace std;

/**
 * Finds the miniumum weighted edge leaving the set of connected vertices the root vertex belongs to.
*/
Edge* findMinOutGoingEdge(Graph& graph, int root_id, DisjointSet& ds) {
    Edge* minEdge = nullptr;
    int minWeight = numeric_limits<int>::max();
    vector<int> connected_vertices = ds.getConnectedIds(root_id); // get vector of vertex id's connected to root
    for(int vertexId: connected_vertices){
        Vertex* currVertex = graph.vertices[vertexId];
        for(Edge* edge: currVertex->edges) {
            if (ds.find(edge->vertex1->id) != ds.find(edge->vertex2->id)) { // if the edge is leaving the connected set
                if (edge->weight < minWeight) {
                    minWeight = edge->weight;
                    minEdge = edge;
                }
            }
        }
    }
    return minEdge;
}

/**
 * Constructs a vector of start,end indices for each process to operate on in the disjoint set.
*/
vector<vector<int>> getProcessRanges(DisjointSet& ds, int world_size) {
    vector<vector<int>> ds_range;
    int min_vertices_per_process = ds.size / world_size;
    int remaining_vertices = ds.size % world_size;
    int start_idx = 0;

    for (int i=0;i<world_size;i++) {
        int vertices_for_curr = min_vertices_per_process + (i < remaining_vertices ? 1 : 0);
        int end_idx = start_idx + vertices_for_curr;
        ds_range.push_back({start_idx, end_idx}); // push the starting and ending indices for the i-th process
        start_idx = end_idx;
    }
    return ds_range;
}

/**
 * Iterate over the designated section of disjoint set and add the minimum weighted leaving edge from each to the local_min_edges vector.
*/
void selectMinEdges(vector<Edge>* local_min_edges, vector<vector<int>>& ds_range, Graph& inputGraph, DisjointSet& ds, int world_rank) {
    for(int i=ds_range[world_rank][0]; i<ds_range[world_rank][1];i++) {
        if(ds.find(i)!=i) { // if the current vertex is not the root of it's connected subgraph, continue
            continue;
        }
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, i, ds);
        if (min_leaving_edge==nullptr){
            continue;
        }
        local_min_edges->push_back(*min_leaving_edge);
    }
}

/**
 * Serialize the local_min_edges and gather them in the root process (world_rank = 0).
 * The goal is to populate a global_min_edges with candidate edges for the MST from all processes.
*/
void gatherEdges(vector<Edge>* global_min_edges, vector<Edge>& local_min_edges, Graph& inputGraph, int world_rank, int world_size) {
    // serialize each property of the edges into their own array
    int local_weights[local_min_edges.size()];
    int local_vertex1s[local_min_edges.size()];
    int local_vertex2s[local_min_edges.size()];
    for (int i=0;i<local_min_edges.size();i++) {
        local_weights[i] = local_min_edges[i].weight;
        local_vertex1s[i] = local_min_edges[i].vertex1->id;
        local_vertex2s[i] = local_min_edges[i].vertex2->id;
    }

    int local_count = local_min_edges.size();
    int* counts = nullptr; // holds the length of local_min_edges for each process
    int* displacements = nullptr; // holds the starting index for where to copy each of the edges arrays 
    if (world_rank==0) {
        counts = new int[world_size];
        displacements = new int[world_size];
    }

    // store the respective sizes of the arrays being sent to process 0 from process i 
    MPI_Gather(&local_count, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // calc the displacements in the global array based on the counts 
    if (world_rank==0) {
        displacements[0] = 0;
        for (int i=1;i<world_size;i++) {
            displacements[i] = displacements[i-1] + counts[i-1];
        }
    }

    // sum the counts to get the size of the global edge array
    int total_count;
    if (world_rank==0) {
        total_count = std::accumulate(counts, counts+world_size, 0);
    } else {
        total_count = 0;
    }

    // define arrays to receive the properties of the local process edges
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

    // gather the properties of the local edges to each process i in process 0
    MPI_Gatherv(local_weights, local_count, MPI_INT,
                all_weights, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Gatherv(local_vertex1s, local_count, MPI_INT,
                all_vertex1s, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);
    MPI_Gatherv(local_vertex2s, local_count, MPI_INT,
                all_vertex2s, counts, displacements, MPI_INT,
                0, MPI_COMM_WORLD);

    // construct edges from the properties received in the gather step
    if (world_rank==0) {
        for (int i=0;i<total_count;i++) {
            Vertex* vertex1 = inputGraph.vertices[all_vertex1s[i]];
            Vertex* vertex2 = inputGraph.vertices[all_vertex2s[i]];
            Edge* edge = new Edge(vertex1, vertex2, all_weights[i]);
            global_min_edges->push_back(*edge);
        }
    }

    // clean up allocated memory
    if (world_rank == 0) {
        delete[] counts;
        delete[] displacements;
        delete[] all_weights;
        delete[] all_vertex1s;
        delete[] all_vertex2s;
    }
}


/**
 * 
*/
vector<Edge> distributedPrims(Graph& inputGraph, int world_size, int world_rank) {
    chrono::time_point<std::chrono::high_resolution_clock> start;
    if (world_rank==0) {
        start = chrono::high_resolution_clock::now();
    }
    
    DisjointSet ds(inputGraph.V); // initially stores one set for each vertex, merged later through merge operations
    vector<Edge> mst; 
    vector<Edge> local_min_edges;
    vector<Edge> global_min_edges;
    bool edges_remaining = true;
    
    // define the ranges within the ds allocated to the respective processes
    vector<vector<int>> ds_range = getProcessRanges(ds, world_size);

    // construct a vector of min leaving edges for each process
    selectMinEdges(&local_min_edges, ds_range, inputGraph, ds, world_rank);

    // gather all local_min_edges from process i to process 0
    gatherEdges(&global_min_edges, local_min_edges, inputGraph, world_rank, world_size);

    // sort the edges by weight min to max
    sort(global_min_edges.begin(), global_min_edges.end());

    local_min_edges.clear();

    // loop while there are still edges in the global_min_edges vector
    while(edges_remaining){
        // add edges to the MST and merge their vertex sets in the disjoint set
        if (world_rank==0) {
            for (int i=0; i<global_min_edges.size();i++) {
                if(ds.find(global_min_edges[i].vertex1->id) != ds.find(global_min_edges[i].vertex2->id)) { // if the edge is out-going
                    ds.merge(global_min_edges[i].vertex1->id, global_min_edges[i].vertex2->id);
                    mst.push_back(global_min_edges[i]);
                }
            }
            // clear vector since all edges were checked
            global_min_edges.clear();
        }

        // send the updated disjoint set to all processes from the root process as it is the only process with the true set
        MPI_Bcast(&ds.parent[0], ds.size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        // construct a vector of min leaving edges for each process
        selectMinEdges(&local_min_edges, ds_range, inputGraph, ds, world_rank);

        // gather all local_min_edges from process i to process 0
        gatherEdges(&global_min_edges, local_min_edges, inputGraph, world_rank, world_size);

        local_min_edges.clear();


        // check to see if we are going to repeat the loop.
        if (global_min_edges.empty() && world_rank==0) {
            edges_remaining = false;
        }
        // let the other processes know if edges are still remaining.
        MPI_Bcast(&edges_remaining, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (edges_remaining) {
            sort(global_min_edges.begin(), global_min_edges.end());
        }
    }

    if (world_rank==0) {
        auto end = chrono::high_resolution_clock::now();
        double elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        cout << "Elapsed time: " << elapsed << " ms" << endl;
    }

    return mst;
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
        if (testVerificationLogs) {
            if (mst.size() != g.V-1) {
                cout<<"WARNING: MST does not contain V-1 edges."<<endl;
            }else{
                cout<<"TEST LOG: Graph size is valid."<<endl;
            }
            if (!g.isConnected()) {
                cout<<"WANRING: MST is not connected."<<endl;
            }else{
                cout<<"TEST LOG: Graph is connected."<<endl;
            }
        }
    }
    MPI_Finalize();
    return 0;
}