#include "utils/graph.h"

int main(int argc, char *argv[]) {
    Graph graph1("inputGraphs/bitcoin.txt");
    cout << "List of edges:" << endl;
    for (const Edge& edge : graph1.edges) {
        cout << "Node " << edge.first << " to Node " << edge.second << ", Weight: " << edge.weight << endl;
    }
    return 0;
}