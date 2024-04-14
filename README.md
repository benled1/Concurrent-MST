# CMPT431-Project

## How to run
### Makefile
To use the make file
```
make all
```
This will make all the files needed to run the different implemenations of MST. The different executables.<br>
To make the files individually
```
make mst_serial
```
```
make mst_thread
```
```
make mst_mpi
```
### C++ files
To run those files
```
./mst_serial <graph path> <bool for printing logs>
```
```
./mst_thread <n tasks> <graph path> <bool for printing logs>>
```
```
./mst_mpi <graph path> <bool for printing logs>
```
The graph path is the path to whatever `csv` file is being used as the input graph. We have a folder called `inputGraphs` that holds all our test graphs and a script to creat more.
### Create additional graphs
To create a new graph use the python file `createGraph` like so
```
python3 inputGraphs/createGraph.py
```
After that follow the input steps from the terminal. This will create a new connected graph that can be used with the `c++` files.
### Running in slurm
To run the files in `Slurm`, the bash scripts are needed. They are all created with the file names `serial.sh, thread.sh,mpi.sh`. They run each of the different implementations. There are preset settings but they can be changed as needed. <br>
Example on how to run
```
sbatch serial.sh
``` 
All of the scripts output to a text file labelled `test_<implementation>.txt` with implemenation being the chosen implementation. 
### Evaluation script
To test and compare all the implementations at once, use the script `evaluate_procs.py`. This will run all three in Slurm and then output the recorded times from the `.txt.` files. If you want to adjust the testing parameters do not change this script, instead change the `.sh` files. <br>
Run command
```
python3 evaluate_procs.py
```
This will take a variable amount of time so let it run until all jobs are done. Note: make sure to check that no slurm job failed if the script is taking a considerable amount of time, this will result in an infinite loop of waiting for an update.
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