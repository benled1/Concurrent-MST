#include "utils/graph.h"
#include <condition_variable>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <algorithm>

using namespace std;
std::mutex graphMutex;
std::vector<std::mutex> vertexMutexes;
std::vector<std::mutex> mergeMutexes;

void mergeTree(int thread1, int thread2, std::vector<std::vector<Edge>>& threadMSTVec, std::vector<bool>& restartVec){
    if(thread1 < thread2){
        cout << "SOMETHING WENT WRONG WITH THREAD ORDERING IN MERGE" << endl;
        int temp = thread1;
        thread1 = thread2;
        thread2 = temp;
    }
    // Only merge bigger threads tree into smaller thread
    threadMSTVec[thread2].insert(threadMSTVec[thread2].end(), threadMSTVec[thread1].begin(), threadMSTVec[thread1].end());
    std::sort(threadMSTVec[thread2].begin(), threadMSTVec[thread2].end());
    std::unordered_set<Edge, EdgeHasher> edgesSet;
    for(Edge e : threadMSTVec[thread2]){
        e.vertex1->color = thread2;
        e.vertex2->color = thread2;
        edgesSet.insert(e);
    }
    bool hasDuplicates = edgesSet.size() != threadMSTVec[thread2].size();;
    if(hasDuplicates){
        // Remake vector if it has dupes
        threadMSTVec[thread2].clear();
        threadMSTVec[thread2].insert(threadMSTVec[thread2].end(), edgesSet.begin(), edgesSet.end());
    }
    // Remove from old thread mst
    threadMSTVec[thread1].clear();
    restartVec[thread1] = true;
}

void mstThread(int tid, Graph &g, std::vector<std::vector<Edge>>& threadMSTVec, std::vector<bool>& restartVec){
    while(true)
    {
        priority_queue<Edge> edge_pq;
        graphMutex.lock();
        Vertex* starting_vertex = g.getRandomUnvisitedVertex();
        graphMutex.unlock();
        cout << tid << " grabbed " << starting_vertex->id << endl;
        if(starting_vertex == nullptr){
            return;
        }
        // Add all adjacent edges to the PQ
        for (Edge* edge : starting_vertex->edges) {
            edge_pq.push(*edge);
        }
        // Keep going until we have no more to go through
        while(!edge_pq.empty()){
            if(restartVec[tid] == true){
                // do something now that they have to restart with a new vertex
                restartVec[tid] == false;
                break;
            }
            Edge current_edge = edge_pq.top();
            edge_pq.pop();
            Vertex* targetVertex;
            if(current_edge.vertex1->color == -1){
                targetVertex = current_edge.vertex1;
            }
            else{
                targetVertex = current_edge.vertex2;
            }

            vertexMutexes[targetVertex->id].lock();
            int targetColor = targetVertex->color;
            cout << targetVertex->id << " : " << targetColor << endl;
            if(targetColor == -1){
                //Unvisited
                targetVertex->color = tid;
                
                mergeMutexes[tid].lock();
                threadMSTVec[tid].push_back(current_edge);
                mergeMutexes[tid].unlock();

                for (Edge* edge : targetVertex->edges) {
                    if(edge->vertex1->color == -1 || edge->vertex2->color == -1 ){
                        edge_pq.push(*edge);
                    }
                }
                // Unblock everyone
                vertexMutexes[targetVertex->id].unlock();
            }
            else if(targetColor != tid){
                // Visited by another thread
                int visitedThread = targetColor;
                if(visitedThread > tid){
                    mergeMutexes[tid].lock();
                    mergeMutexes[visitedThread].lock();

                    mergeTree(visitedThread, tid, std::ref(threadMSTVec), std::ref(restartVec));

                    mergeMutexes[tid].unlock();
                    mergeMutexes[visitedThread].unlock();
                }
                else if (visitedThread > tid){
                    mergeMutexes[visitedThread].lock();
                    mergeMutexes[tid].lock();
                    
                    mergeTree(tid, visitedThread, std::ref(threadMSTVec), std::ref(restartVec));
                    
                    mergeMutexes[visitedThread].unlock();
                    mergeMutexes[tid].unlock();
                }
                // Unlock the mutex
                vertexMutexes[targetVertex->id].unlock();
            }
            else if (targetColor == tid){
                //Visited by self
                // Unlock the mutex
                vertexMutexes[targetVertex->id].unlock();
            }
        } 
        break;  
    }
    
}

void mstSerial(int nthreads, Graph g){
    size_t v = (size_t) g.V;
    std::vector<std::mutex> list1(v);
    vertexMutexes.swap(list1);
    std::vector<std::mutex> list2(nthreads);
    mergeMutexes.swap(list2);
    std::thread threads[nthreads];
    std::vector<bool> restartVec(nthreads);
    std::vector<std::vector<Edge>> threadMSTVec(nthreads);
    cout << "Starting Threads" << endl;
    for(int i = 0; i < nthreads; i++){
       threads[i] = std::thread(mstThread, i, std::ref(g), std::ref(threadMSTVec), std::ref(restartVec));
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