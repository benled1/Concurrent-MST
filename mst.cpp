#include "utils/graph.h"

using namespace std;

int minKey() {

}

vector<Edge> primMST(Graph inputGraph) {
    // priority queue of all the edges in the graph
    priority_queue<Edge> edge_pq;

    // random starting vertex
    int starting_vertex = 0;

    // keep track of vertexes in MST
    vector<bool> included_vertices(V, false);

    // num of edges included in the MST
    int num_included_edges = 0;

    // add all the edges that are connected to the starting vertex to the pq
    for (Edge& edge : inputGraph.edges) {
        if (edge.first == starting_vertex || edge.second == starting_vertex) {
            pq.push(edge);
        }
    }

    included_vertices[start] = true;

    // Vector to store the resultant MST
    vector<Edge> result;

    while (!pq.empty() && num_included_edges < inputGraph.V-1) {
        Edge shortest_edge = pq.top();
        pq.pop();

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
            for (Edge& edge: inputGraph.edges) {
                if (edge.first == target || edge.second == target) {
                    if(!included_vertices[edge.first] || !included_vertices[edge.second]) {
                        pq.push(edge);
                    }
                }
            }
        }


    }

    //Print the MST
    for (Edge& edge: result) {
        cout << edge.first << " - " << edge.second << " : " << edge.weight << endl;
    }   

}

int main(int argc, char *argv[]) {
    Graph inputGraph = Graph("inputGraphs/testData.csv");
    cout << "List of edges:" << endl;
    for (const Edge& edge : inputGraph.edges) {
        cout << "Node " << edge.first << " to Node " << edge.second << ", Weight: " << edge.weight << endl;
    }
    cout << inputGraph.hasDuplicateEdges() << endl;
    vector<Edge> mst = primMST(inputGraph);
}