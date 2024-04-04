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
if process 0:
    subgraphs_set = ((0), (1), ..., (n)) // a set of sets of vertices
    mst = []

    split_edge_list_by_process() // divide the edge list into only edges that touch the vertices for each process

broadcast(subgraphs_set)

in parallel:
    min_edges = []
    for each subgraph in partition(subgraphs_set):
        min_leaving_edge = find_min_leaving_edge(subgraph) // get the min leaving edge from the subgraph
        min_edges.append(min_leaving_edge)
    send(min_edges, process0) // send the min_edges from each process to process 0

if process 0:
    rcv(all_min_edges)
    min_edges = combine(all_min_edges)
    sort(min_edges) // want to pull the lowest weight min edge first

while len(subgraphs_set) > 1:
    if process 0:
        for each min_edge in min_edges:
            if min_edge connects two disjoint subgraphs:
                subgraphs_set.merge_subgraphs()
                mst.append(min_edge)
                min_edge.pop() //remove the one we just processed.
        broadcast(subgraphs_set)
    in parallel: // find the min outgoing edges on the new subgraphs_set
        subgraphs_set = rcv(subgraphs_set) // update the subgraphs_set based on process0 broadcast
        min_edges = []
        for each subgraph in partition(subgraphs_set):
            min_leaving_edge = find_min_leaving_edge(subgraph) // get the min leaving edge from the subgraph
            min_edges.append(min_leaving_edge)
        send(min_edges, process0) // send the min_edges from each process to process 0

return mst
```