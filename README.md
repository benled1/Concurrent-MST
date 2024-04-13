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

### Threaded

```
threaded
for each node
	color = -1
	
create an array of locks for each node
	
create threads

in parallel:
	create a priority queue
	lock graph;
	start_vertex = random uncoloured node from the graph
	unlock graph;
	
	for each edge connected to start_vertex
		add edge e to priority queue 
	
	create an array to hold MST edges
	
	while priority queue not empty
		currentEdge = pq.pop()
		if (edge.first_node.color != -1)
			targetNode = first_node
		else
			targetNode = second_node
		
		lock MST_lock_array[targetNode.index]
		
		if(targetNode.color == -1)
			block all threads
			
			targetNode.color = threadID
			
			add edge to MST
			
			for each edge connected to targetNode
				if(edge has atleast 1 node not visited)
					add edge e to priority queue 
			unlock MST_lock_array[targetNode.index]
			unblock all threads
		
		else if (targetNode.color != threadID)
			
			visitedThread = targetNode.color
			
			if(visitedThread < threadID)
				lock threadID and visitedThread
				
				mergeTree(visitedThread, threadID)
				
				unlock MST_lock_array[targetNode.index]
				unlock threadID and visitedThread
			
			else if(threadID < visitedThread)	
				lock threadID and visitedThread
				
				mergeTree(threadID, visitedThread)
				
				unlock MST_lock_array[targetNode.index]
				unlock threadID and visitedThread
		
		else if(targetNode.color == threadID)
			unlock MST_lock_array[targetNode.index]
			
			
			
function mergeTree(t1, t2)
	merge t2's edge list into t1
	update the size and weight of the new MST
	color all vertices in the MST with t1's id
	update the priority queue for i
	remove all edges from t2's list

```



### MPI

```
DistributedPrims(Graph inputGraph, int world_size, int world_rank) {

    DisjointSet ds(num_vertices);
    vector<Edge> mst;
    vector<Edge> global_min_edges;
    bool edges_remaining = true;

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

    while(edges_remaining==true) {

        for(every edge in min_edges) {
            if(edge span across two disjoint subsets in ds) {
                ds.merge(vertex1.id, vertex2.id); // merge the sets that are spanned by the edge
                mst.push_back(edge); // add the edge to the mst
            } 
        }

        global_min_edges.clear(); // empty the min_edges vector

        broadcast(ds);
    
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
        
        if (global_min_edges.empty()) {
            edges_remaining = false;
        }

        Barrier() // wait on all the processes to be ready for the next loop
    }
    return mst;
}
```