#include "utils/graph.h"
using namespace std;
int main(int argc, char *argv[]) {
    cout << "Starting Parsing" << endl;
    Graph graph1("inputGraphs/bitcoin.csv");
    cout << "List of edges:" << endl;
    for (const Edge& edge : graph1.edges) {
        cout << "Node " << edge.first << " to Node " << edge.second << ", Weight: " << edge.weight << endl;
    }
    return 0;
}