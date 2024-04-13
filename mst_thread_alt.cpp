#include "utils/graph.h"
#include "utils/disjoint_set.h"
#include <cassert>
#include <algorithm>
#include <string>
#include <limits>
#include <mutex>
#include <thread>
#include <mutex>

using namespace std;
atomic<bool> mstDone(false);
mutex pqMtx;
mutex mstMtx;
mutex dsMtx;

void mstThread(int tid, Graph& g, DisjointSet& ds, vector<Edge>& mst, priority_queue<Edge>& pq){
    // terminate at some condition
    while(true){
        pqMtx.lock();
        if(pq.empty())
        {
            if(mst.size() == g.V - 1){
                mstDone = true;
            }
            pqMtx.unlock();
            if (mstDone){ 
                break;
            }
            else{
                continue;
            }
        }
        Edge current_edge = pq.top();
        cout << current_edge.vertex1->id << "--" << current_edge.vertex2->id << " " << tid << endl; 
        pq.pop();
        pqMtx.unlock();
        dsMtx.lock();
        int id1 = current_edge.vertex1->id;
        int id2 = current_edge.vertex2->id;
        if(ds.find(id1) != ds.find(id2)){
            mstMtx.lock();
            mst.push_back(current_edge);
            mstMtx.unlock();
            ds.merge(id1, id2); 
            dsMtx.unlock();
            pqMtx.lock();
            for (Edge* e : current_edge.vertex1->edges) {
                int otherVertexId = (e->vertex1->id == id1) ? e->vertex2->id : e->vertex1->id;
                dsMtx.lock();
                if (ds.find(id1) != ds.find(otherVertexId))
                {
                    pq.push(*e);
                }
                dsMtx.unlock();
            }
            for (Edge* e : current_edge.vertex2->edges) {
                int otherVertexId = (e->vertex1->id == id2) ? e->vertex1->id  : e->vertex2->id ;
                dsMtx.lock();
                if (ds.find(id2) != ds.find(otherVertexId))
                {
                    pq.push(*e);
                }
                dsMtx.unlock();
            }
            pqMtx.unlock();
        }
        else{
            dsMtx.unlock();
        }
    }

}

vector<Edge> serialPrims(Graph& inputGraph, int n_threads) {
    clock_t start = clock();

    // init the disjoint set, final ms and threads
    DisjointSet ds(inputGraph.V);
    vector<Edge> mst;
    thread threads[n_threads];

    // Create the initial values for the pq
    priority_queue<Edge> edge_pq;
    Vertex* starting_vertex = inputGraph.vertices[0];
    for (Edge* edge : starting_vertex->edges) {
        edge_pq.push(*edge);
    }
    cout << "PQ Size" << edge_pq.size() << endl;
    for(int i = 0; i < n_threads; ++i){
        cout << "Thread #" << i << " started" << endl;
        threads[i] = thread(mstThread, i, std::ref(inputGraph), std::ref(ds), std::ref(mst), std::ref(edge_pq));
    }
    for(int i = 0; i < n_threads; ++i){
        threads[i].join();
    }

    clock_t end = clock();
    // convert to milliseconds
    double elapsed = double(end - start) / CLOCKS_PER_SEC * 1000; 
    cout << "Elapsed time: " << elapsed << " ms" <<endl;

    return mst;
}


int main(int argc, char *argv[]){
     if (argc <= 1) {
        cout<<"mst_threads requires at least a filepath."<<endl;
        return 0;
    }
    bool testVerificationLogs = false;
    string graphFilePath = argv[1];
    if (argc >= 3) {
        testVerificationLogs = argv[2];
    }

    Graph g;
    g.readCSV(graphFilePath);
    //TODO: Add to args
    int n_threads = 4;
    vector<Edge> mst = serialPrims(g, n_threads);

    cout<<"MST:"<<endl;
    for(Edge edge: mst) {
        cout<<edge.vertex1->id<<"-"<<edge.vertex2->id<<": Weight="<<edge.weight<<endl;
    }

    return 0;
}


