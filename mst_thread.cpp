#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <limits>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;

mutex minEdgesMutex;

/**
 * Finds the minimum weighted edge leaving the set of connected vertices the root vertex belongs to.
 */
Edge* findMinOutGoingEdge(Graph& graph, int root_id, DisjointSet& ds) {
    Edge* minEdge = nullptr;
    int minWeight = numeric_limits<int>::max();
    vector<int> connected_vertices = ds.getConnectedIds(root_id);
    for(int vertexId: connected_vertices){
        Vertex* currVertex = graph.vertices[vertexId];
        for(Edge* edge: currVertex->edges) {
            if (ds.find(edge->vertex1->id) != ds.find(edge->vertex2->id)) {
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
 * Thread function to select the min weighted outgoing edge from each connected set.
 */
void selectMinEdgesThread(vector<Edge>* min_edges, Graph& inputGraph, DisjointSet& ds, int start, int end) {
    vector<Edge> localMinEdges;
    for(int i=start; i<end; i++) {
        if(ds.find(i)==i) {
            Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, i, ds);
            if (min_leaving_edge != nullptr) {
                localMinEdges.push_back(*min_leaving_edge);
            }
        }
    }
    // lock the mutex when adding edges to the global vector of edges
    lock_guard<mutex> lock(minEdgesMutex);
    min_edges->insert(min_edges->end(), localMinEdges.begin(), localMinEdges.end());
}

vector<Edge> parallelPrims(Graph& inputGraph, int numThreads) {
    auto start = chrono::high_resolution_clock::now();
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> min_edges;
    vector<vector<int>> thread_ranges;

    vector<thread> threads;
    int min_vertices_per_thread = ds.size / numThreads;
    int remaining_vertices = ds.size % numThreads;
    int startIdx = 0;

    // distribute sections of the ds to different threads
    for (int i = 0; i < numThreads; i++) {
        int vertices_for_curr = min_vertices_per_thread + (i < remaining_vertices ? 1 : 0);
        int endIdx = startIdx + vertices_for_curr;
        thread_ranges.push_back({startIdx, endIdx});
        startIdx = endIdx;
    }

    for (int i=0; i<numThreads;i++) {
        threads.emplace_back(selectMinEdgesThread, &min_edges, ref(inputGraph), ref(ds), thread_ranges[i][0], thread_ranges[i][1]);
    }

    for (thread& th : threads){
        th.join();
    } 
    threads.clear();

    sort(min_edges.begin(), min_edges.end());


    // loop while min_edges is empty
    while (!min_edges.empty()) {
        // loop over the min edges found and merge them in the disjoint set. then push these edges to final mst
        for (Edge& edge : min_edges) {
            if (ds.find(edge.vertex1->id) != ds.find(edge.vertex2->id)) {
                ds.merge(edge.vertex1->id, edge.vertex2->id);
                mst.push_back(edge);
            }
        }
        min_edges.clear();

        // find new set of min outgoing edges
        for (int i=0; i<numThreads;i++) {
            threads.emplace_back(selectMinEdgesThread, &min_edges, ref(inputGraph), ref(ds), thread_ranges[i][0], thread_ranges[i][1]);
        }
        for (thread& th : threads) th.join();
        threads.clear();

        sort(min_edges.begin(), min_edges.end());
    }

    auto end = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    cout << "Elapsed time: " << elapsed << " ms" << endl;

    return mst;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "mst_parallel requires at least a filepath." << endl;
        return 0;
    }

    string graphFilePath = argv[1];
    int numThreads;
    if (argc >= 3) {
        numThreads = stoi(argv[2]);
    }
    bool testVerificationLogs=false;
    if (argc >= 4) {
        testVerificationLogs = argv[3];
    }

    Graph g;
    g.readCSV(graphFilePath);
    vector<Edge> mst = parallelPrims(g, numThreads);
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
    return 0;
}
