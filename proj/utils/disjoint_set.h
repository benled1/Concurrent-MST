#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <cassert>
// #include "graph.h"

using namespace std;

class DisjointSet {
public:
    std::vector<int> parent; // Parent of each element's index
    int size;

    DisjointSet(const int set_size) {
        size = set_size;
        parent.resize(size);

        // init the parent array with index representing the vertex id and value the parent vertex id
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    // find the root vertex of the set that contains vertex with id u
    int find(int u) {
        if (parent[u] == u) {
            return u;
        }
        else {
            return find(parent[u]);
        }
    }

    // union the sets that contain u and v vertex id's respectively
    void merge(int u, int v) {
        int urep = find(u);
        int vrep = find(v);

        parent[urep] = vrep;
    }

    // return a list of vertex id's that are connected to the vertex with id u
    std::vector<int> getConnectedIds(int u) {
        std::vector<int> connectedIds;
        int root = find(u);

        for (int i = 0; i<size;i++) {
            if (find(i) == root) {
                connectedIds.push_back(i);
            }
        }
        return connectedIds;
    }

};

void testConstructor() {
    DisjointSet ds(9);
    for (int i=0;i<9;i++) {
        assert(ds.parent[i]==i);
    }
    cout<<"Constructor test passed"<<endl;
}

void testMerge() {
    DisjointSet ds(9);
    ds.merge(1,4);
    ds.merge(2,6);
    ds.merge(4,2);
    assert(ds.find(1)==6);
    cout<<"Merge test passed"<<endl;
}

void testFind() {
    DisjointSet ds(9);
    ds.merge(1,4);
    assert(ds.find(1)==4);
    cout<<"Find test passed"<<endl;
}

void testGetConnectedIds(){
    DisjointSet ds(9);
    ds.merge(1,4);
    ds.merge(2,6);
    ds.merge(4,2);
    vector<int> connectedToOne = ds.getConnectedIds(1);
    cout<<"Connected vertices include:"<<endl;
    for (int id : connectedToOne) {
        cout<<id<<endl;
    }
    cout<<"GetConnectedIds test passed"<<endl;
}

