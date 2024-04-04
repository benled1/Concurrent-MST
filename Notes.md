# Ideas for parallelizing prims

# notes on the distributed version in 5.3 on the stanford paper
- first create a set of all of vertices. Each vertex is on it's own 
    - think `((a),(b),(c))` where each vertex is in own disjoint set.
- in parallel across multiple machines, find the smallest edge leaving each disjoint set.
- for each minimum edge found, if it connects two different disjoint sets, the edge is added to the MST (this is global)
    - then the two disjoint sets are unified. 
- after each iteration of adding edges and merging disjoint sets, the updated disjoint set structure is broadcasted to all machines. 
    - this makes sure that each machine keeps all its information up to date on which vertices are connected.
- repetition until completion: repeat steps 2-4 until there are no more edges to consider which happens when all vertices are connected forming the MSt.
    - basically once the original disjoint sets of V form a single set, we can stop.
- the set T, (the set of edges we have been adding to) will contain all the edges added during the algorithm represents the MST of the graph.


Below is some psuedo code
```
subgraphs_set = ((0), (1), ..., (n)) // a set of sets of vertices
mst = []
in parallel:
    while(len(subgraphs_set) > 1) // while the subgraphs are not fully connected
        min_edges = []
        for each subgraph in partition(subgraphs_set):
            min_leaving_edge = find_min_leaving_edge(subgraph) // get the min leaving edge from the subgraph
            min_edges.append(min_leaving_edge)
        
        sort(min_edges) // want to pull the lowest weight min edge first

        for each min_edge in min_edges:
            if min_edge connects two disjoint subgraphs:
                subgraphs_set.merge_subgraphs()
                mst.append(min_edge)
return mst
```