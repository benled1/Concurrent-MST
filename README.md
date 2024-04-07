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