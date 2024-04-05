#include "utils/graph.h"
#include <queue>

using namespace std;

void primMST(Graph* inputGraph) {
    // priority queue of all the edges in the graph
    priority_queue<Edge> edge_pq;

    // random starting vertex
    Vertex& starting_vertex = inputGraph.getRandomUnvisitedVertex();

    // keep track of vertexes in MST
    vector<bool> included_vertices(inputGraph->V, false);

    // num of edges included in the MST
    int num_included_edges = 0;

    // add all the edges that are connected to the starting vertex to the pq
    for (Edge& edge : inputGraph->edges) {
        if (edge.first == starting_vertex || edge.second == starting_vertex) {
            edge_pq.push(edge);
        }
    }

    included_vertices[starting_vertex.id] = true;

    // Vector to store the resultant MST
    vector<Edge> result;

    while (!edge_pq.empty() && num_included_edges < inputGraph->V-1) {
        Edge shortest_edge = edge_pq.top();
        edge_pq.pop();

        // pick the shortest edge from the priority queue to include in the mst. make sure that 
        Vertex target;
        if (included_vertices[shortest_edge.first.id]) {
            target = shortest_edge.second;
        }else{
            target = shortest_edge.first;
        }


        // if the target vertex has NOT already been visited/included in MST 
        if(!included_vertices[target.id]) {
            included_vertices[target.id] = true;
            result.push_back(shortest_edge);
            num_included_edges++;

            // add all edges of the newly included vertex to the priority queue
            for (Edge& edge: inputGraph->edges) {
                if (edge.first == target || edge.second == target) {
                    if(!included_vertices[edge.first.id] || !included_vertices[edge.second.id]) {
                        edge_pq.push(edge);
                    }
                }
            }
        }


    }

    cout << "MST:" << endl;
    //Print the MST
    for (Edge& edge: result) {
        cout << edge.first.id << " - " << edge.second.id << " : " << edge.weight << endl;
    }   

}

int main(int argc, char *argv[]) {
    Graph inputGraph = Graph("inputGraphs/testData.csv");
    cout << "List of edges:" << endl;
    for (const Edge& edge : inputGraph.edges) {
        cout << "Node " << edge.first.id << " to Node " << edge.second.id << ", Weight: " << edge.weight << endl;
    }
    cout << inputGraph.V << endl;
    cout << inputGraph.hasDuplicateEdges() << endl;

    primMST(&inputGraph);
    
}