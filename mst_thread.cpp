#include "utils/graph.h"
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

using namespace std;
std::mutex graphMutex;
std::vector<std::mutex> vertexMutexes;

void mstThread(int tid, Graph &g, std::vector<std::vector<Edge>>& threadMSTVec){
    priority_queue<Edge> edge_pq;
    cout << "PQ Made" << endl;
    graphMutex.lock();
    cout << threadMSTVec.size() << " : " << tid << endl;
    std::vector<Edge> threadMST = threadMSTVec[tid];
    cout << "Single MST accessed" << endl;
    Vertex* starting_vertex = g.getRandomUnvisitedVertex();
    cout << starting_vertex->id << endl;
    graphMutex.unlock();
    
}

void mstSerial(int nthreads, Graph g){
    size_t v = (size_t) g.V;
    std::vector<std::mutex> list(v);
    vertexMutexes.swap(list);
    std::thread threads[nthreads];
    std::vector<std::vector<Edge>> threadMSTVec(nthreads);
    for(int i = 0; i < nthreads; i++){
       threads[i] = std::thread(mstThread, i, std::ref(g), std::ref(threadMSTVec));
    }
    for(int i = 0; i < nthreads; i++){
        threads[i].join();
    }
    return;
}

int main(int argc, char *argv[]){
    Graph g;
    g.readCSV("./inputGraphs/connected_graph.csv");
    string connectionTest = g.isConnected() ? "Graph is connected" : "Graph is not connected";
    cout << connectionTest << endl;
    int n_threads = 2;
    mstSerial(n_threads, g);
    return 0;
}