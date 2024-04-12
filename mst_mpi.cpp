#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <numeric>
#include <limits>
#include </usr/include/mpi/mpi.h>
#include <ctime>

using namespace std;

/**
 * Finds the miniumum weighted edge leaving the set of connected vertices the root vertex belongs to.
*/
Edge* findMinOutGoingEdge(Graph& graph, int root_id, DisjointSet& ds) {
    Edge* minEdge = nullptr;
    int minWeight = numeric_limits<int>::max();
    vector<int> connected_vertices = ds.getConnectedIds(root_id);
    for(int vertexId: connected_vertices){
        Vertex* currVertex = graph.vertices[vertexId];
        for(Edge* edge: currVertex->edges) {
            // check if the edge is leaving the connected set
            if (ds.find(edge->vertex1->id) != ds.find(edge->vertex2->id)) {
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

/**
 * Constructs a vector of start,end indices for each process to operate on in the disjoint set.
*/
vector<vector<int>> getProcessRanges(DisjointSet& ds, int world_size) {
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
    return ds_range;
}

void selectMinEdges(vector<Edge>* local_min_edges, vector<vector<int>>& ds_range, Graph& inputGraph, DisjointSet& ds, int world_rank) {
    // in parallel loop over portion that was allocated
    for(int i=ds_range[world_rank][0]; i<ds_range[world_rank][1];i++) {
        if(ds.find(i)!=i) {
            continue;
        }
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, i, ds);
        if (min_leaving_edge==nullptr){
            continue;
        }
        local_min_edges->push_back(*min_leaving_edge);
    }
}


// below is the distributed version of prims 
vector<Edge> distributedPrims(Graph& inputGraph, int world_size, int world_rank) {
    clock_t start;
    if (world_rank==0) {
        start = clock();
    }
    
    // init the disjoint set, final mst, and min_edges
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> local_min_edges;
    vector<Edge> global_min_edges;
    bool edges_remaining = true;
    
    // define the ranges within the ds allocated to the respective processes
    vector<vector<int>> ds_range = getProcessRanges(ds, world_size);

    // construct a vector of min leaving edges for each process
    selectMinEdges(&local_min_edges, ds_range, inputGraph, ds, world_rank);

    // serialize the edge data into arrays to send over MPI
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
            Vertex* vertex1 = inputGraph.vertices[all_vertex1s[i]];
            Vertex* vertex2 = inputGraph.vertices[all_vertex2s[i]];
            Edge* edge = new Edge(vertex1, vertex2, all_weights[i]);
            global_min_edges.push_back(*edge);
        }
    }

    sort(global_min_edges.begin(), global_min_edges.end());

    if (world_rank == 0) {
        delete[] counts;
        delete[] displacements;
        delete[] all_weights; // When done processing
        delete[] all_vertex1s;
        delete[] all_vertex2s;
    }
    local_min_edges.clear();

    // loop while there are still edges in the min_edges vector
    while(edges_remaining){
        if (world_rank==0) {
            for (int i=0; i<global_min_edges.size();i++) {
                if(ds.find(global_min_edges[i].vertex1->id) != ds.find(global_min_edges[i].vertex2->id)) {
                    ds.merge(global_min_edges[i].vertex1->id, global_min_edges[i].vertex2->id);
                    mst.push_back(global_min_edges[i]);
                }
            }
            // clear the min_edges
            global_min_edges.clear();
        }
        // NEED TO SEND THE UPDATED MAIN DS TO THE REST OF THE PROCESSES
        MPI_Bcast(&ds.parent[0], ds.size, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        // construct a vector of min leaving edges for each process
        selectMinEdges(&local_min_edges, ds_range, inputGraph, ds, world_rank);

        // build three arrays which can be sent over MPI 
        int local_weights[local_min_edges.size()];
        int local_vertex1s[local_min_edges.size()];
        int local_vertex2s[local_min_edges.size()];
        for (int i=0;i<local_min_edges.size();i++) {
            local_weights[i] = local_min_edges[i].weight;
            local_vertex1s[i] = local_min_edges[i].vertex1->id;
            local_vertex2s[i] = local_min_edges[i].vertex2->id;
        }

        // prepare for gathering
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
                Vertex* vertex1 = inputGraph.vertices[all_vertex1s[i]];
                Vertex* vertex2 = inputGraph.vertices[all_vertex2s[i]];
                Edge* edge = new Edge(vertex1, vertex2, all_weights[i]);
                global_min_edges.push_back(*edge);
            }
        }

        if (world_rank == 0) {
            delete[] counts;
            delete[] displacements;
            delete[] all_weights; // When done processing
            delete[] all_vertex1s;
            delete[] all_vertex2s;
        }
        local_min_edges.clear();


        // check to see if we are going to repeat the loop.
        if (global_min_edges.empty() && world_rank==0) {
            edges_remaining = false;
        }

        MPI_Bcast(&edges_remaining, 1, MPI_INT, 0, MPI_COMM_WORLD);

        if (edges_remaining) {
            sort(global_min_edges.begin(), global_min_edges.end());
        }
    }

    if (world_rank==0) {
        clock_t end = clock();
        // convert to milliseconds
        double elapsed = double(end - start) / CLOCKS_PER_SEC * 1000; 
        cout << "Elapsed time: " << elapsed << " ms"<<endl;
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