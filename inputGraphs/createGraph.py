import random
import csv
import os
import sys

def dfs(graph, node, visited):
    visited[node] = True
    for neighbor in graph[node]:
        if not visited[neighbor]:
            dfs(graph, neighbor, visited)

# Check if the correct number of arguments is provided
if len(sys.argv) != 3:
    print("Usage: python createGraph.py <num_nodes> <num_edges>")
    sys.exit(1)

# Get the number of nodes and edges from command-line arguments
num_nodes = int(sys.argv[1])
num_edges = int(sys.argv[2])

# Count the number of existing graph files in the directory
existing_files = [filename for filename in os.listdir(".") if filename.startswith("graph") and filename.endswith(".csv")]
num_existing_files = len(existing_files)

# Generate a connected graph
edges = set()
graph = {i: [] for i in range(1, num_nodes + 1)}
visited = {i: False for i in range(1, num_nodes + 1)}

# Generate a random spanning tree
for i in range(2, num_nodes + 1):
    parent = random.randint(1, i - 1)
    weight = random.randint(1, 100)
    edges.add((parent, i, weight))
    graph[parent].append(i)
    graph[i].append(parent)

# Add additional random edges to complete the required number of edges
while len(edges) < num_edges:
    nodeA = random.randint(1, num_nodes)
    nodeB = random.randint(1, num_nodes)
    if nodeA != nodeB and (nodeA, nodeB) not in edges:
        edges.add((nodeA, nodeB, random.randint(1, 100)))

# Write to CSV file
filename = 'graph_{}.csv'.format(num_existing_files + 1)
with open(filename, 'w') as csvfile:
    fieldnames = ['nodeA', 'nodeB', 'weight']
    writer = csv.DictWriter(csvfile, fieldnames=fieldnames)

    writer.writeheader()
    for edge in edges:
        writer.writerow({'nodeA': edge[0], 'nodeB': edge[1], 'weight': edge[2]})
 
