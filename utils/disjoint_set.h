#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include "graph.h"


class DisjointSet {
public:
    std::vector<int> parent; // Parent of each element's index

    DisjointSet(const std::vector<Vertex>& vertices) {
        int size = vertices.size();
        parent.resize(size);

        // Initialize mappings for vertices based on their IDs
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    // Find the root of the set in which the vertex with ID `u` is located
    int find(int u) {
        if (parent[u] == u) {
            return u;
        }
        else {
            return find(parent[u]); // Path compression, using Vertex ID
        }
    }

    // Union by rank, using vertex IDs
    void merge(int u, int v) {
        int urep = find(u);
        int vrep = find(v);

        parent[urep] = vrep;
    }
};


void testConstructorAndInitialParent() {
    std::vector<Vertex> vertices = {0, 1, 2, 3, 4};
    DisjointSet ds(vertices);
    for (size_t i = 0; i < vertices.size(); ++i) {
        assert(ds.find(i) == int(i)); // Each element should be its own parent
    }
    std::cout << "Constructor test passed.\n";
}

void testMergeAndFindOperations() {
    std::vector<Vertex> vertices = {0, 1, 2, 3, 4};
    DisjointSet ds(vertices);
    ds.merge(0, 1);
    ds.merge(2, 3);
    ds.merge(1, 2);
    assert(ds.find(0) == ds.find(3)); // since all elements are connected, should have the same root for find
    std::cout << "Merge test passed.\n";
}


