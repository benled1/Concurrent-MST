#include "utils/graph.h"
#include <queue>

using namespace std;

void primMST(Graph* inputGraph) {
    // priority queue of all the edges in the graph
    cout << "Running" << endl;
    priority_queue<Edge> edge_pq;

    // random starting vertex
    int starting_vertex = 0;

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

    included_vertices[starting_vertex] = true;

    // Vector to store the resultant MST
    vector<Edge> result;

    while (!edge_pq.empty() && num_included_edges < inputGraph->V-1) {
        Edge shortest_edge = edge_pq.top();
        edge_pq.pop();

        // check which of the vertices in the shorted edge we didn't just come from
        int target;
        if (included_vertices[shortest_edge.first]) {
            target = shortest_edge.first;
        }else{
            target = shortest_edge.second;
        }


        // if the target vertex has NOT already been visited/included in MST 
        if(!included_vertices[target]) {
            included_vertices[target] = true;
            result.push_back(shortest_edge);
            num_included_edges++;

            // add all edges of the newly included vertex to the priority queue
            for (Edge& edge: inputGraph->edges) {
                if (edge.first == target || edge.second == target) {
                    if(!included_vertices[edge.first] || !included_vertices[edge.second]) {
                        edge_pq.push(edge);
                    }
                }
            }
        }


    }
    cout << result[0].first << endl;
    //Print the MST
    for (Edge& edge: result) {
        cout << edge.first << " - " << edge.second << " : " << edge.weight << endl;
    }   

}

int main(int argc, char *argv[]) {
    Graph inputGraph = Graph("inputGraphs/testData.csv", 9);
    cout << "List of edges:" << endl;
    for (const Edge& edge : inputGraph.edges) {
        cout << "Node " << edge.first << " to Node " << edge.second << ", Weight: " << edge.weight << endl;
    }
    cout << inputGraph.V << endl;
    cout << inputGraph.hasDuplicateEdges() << endl;

    primMST(&inputGraph);
    
    // for (const Edge& edge : mst) {
    //     cout << "Node " << edge.first << " to Node " << edge.second << ", Weight: " << edge.weight << endl;
    // }
}