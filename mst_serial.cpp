#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert> // For assert()
#include <algorithm>
#include <string>
#include <limits>

using namespace std;


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


// below is the serial version of the distributed program (does everything on process 0)
vector<Edge> serialPrims(Graph& inputGraph) {
    clock_t start = clock();
    // init the disjoint set, final mst, and min_edges
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    vector<Edge> min_edges;

    for(int i=0; i<ds.size;i++) {
        if(ds.find(i)!=i) {
            continue;
        }
        Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, i, ds);
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
            Edge* min_leaving_edge = findMinOutGoingEdge(inputGraph, i, ds);
            if (min_leaving_edge==nullptr){
                continue;
            }
            min_edges.push_back(*min_leaving_edge);
        }

        sort(min_edges.begin(), min_edges.end());
    }

    clock_t end = clock();
    // convert to milliseconds
    double elapsed = double(end - start) / CLOCKS_PER_SEC * 1000; 
    cout << "Elapsed time: " << elapsed << " ms"<<endl;

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

    vector<Edge> mst = serialPrims(g);

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