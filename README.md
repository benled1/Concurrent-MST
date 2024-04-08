# CMPT431-Project

## Pseudo-Code

### Serial

```
SerialPrims(Graph inputGraph) {

    DisjointSet ds(num_vertices);
    vector<Edge> mst;
    vector<Edge> min_edges;

    for(every vertex id) {
        // add the minimum weighted edge leaving the set the vertex is a part of
        min_edges.push_back(min_leaving_edge); 
    }

    sort(min_edges); // sort smallest to larget

    while(min_edges contains edges) {

        for(every edge in min_edges) {
            if(edge span across two disjoint subsets in ds) {
                ds.merge(vertex1.id, vertex2.id); // merge the sets that are spanned by the edge
                mst.push_back(edge); // add the edge to the mst
            } 
        }

        min_edges.clear(); // empty the min_edges vector
    
        for(every vertex id) {
            // add the minimum weighted edge leaving the set the vertex is a part of
            min_edges.push_back(min_leaving_edge);
        }
    
    }
    return mst;
}
```





### MPI

```
DistributedPrims(Graph inputGraph, int world_size, int world_rank) {

    DisjointSet ds(num_vertices);
    vector<Edge> mst;
    vector<Edge> global_min_edges;

    // in parallel #################
    vector<Edge> local_min_edges;
    for(every vertex id in my section of ds) {
        // add the minimum weighted edge leaving the set the vertex is a part of
        local_min_edges.push_back(min_leaving_edge); 
    }

    // send the min_edges found to the root process
    if (world_rank!=0) {
        send(local_min_edges, process0);
    } 
    // end in parallel ###############

    // receive all the min_edges from the other processes and add them to global
    for(every process in world_size) {
        recv(gloabl_min_edges, processi);
    }

    sort(global_min_edges); // sort smallest to larget

    while(global_min_edges contains edges) {

        for(every edge in min_edges) {
            if(edge span across two disjoint subsets in ds) {
                ds.merge(vertex1.id, vertex2.id); // merge the sets that are spanned by the edge
                mst.push_back(edge); // add the edge to the mst
            } 
        }

        global_min_edges.clear(); // empty the min_edges vector
    
        // in parallel #################
        vector<Edge> local_min_edges;
        for(every vertex id in my section of ds) {
            // add the minimum weighted edge leaving the set the vertex is a part of
            local_min_edges.push_back(min_leaving_edge); 
        }

        // send the min_edges found to the root process
        if (world_rank!=0) {
            send(local_min_edges, process0);
        } 
        // end in parallel ###############

        // receive all the min_edges from the other processes and add them to global
        for(every process in world_size) {
            recv(global_min_edges, processi);
        }
        
    }
    return mst;
}
```