import random
import csv
import os  # Import os to handle directory creation

class Graph:
    def __init__(self, vertices):
        self.vertices = vertices
        self.edges = []

    def add_edge(self, v1, v2, weight):
        self.edges.append((v1, v2, weight))

    def generate_connected_graph(self):
        # Generate random edges to ensure connectivity
        for i in range(self.vertices - 1):
            self.add_edge(i, i + 1, random.randint(1, 100))

        # Connect the last vertex to the first one to close the loop
        self.add_edge(self.vertices - 1, 0, random.randint(1, 100))

    def write_to_csv(self, filename):
        with open(filename, 'w', newline='') as csvfile:  # 'newline=''' to avoid extra new lines in csv files
            writer = csv.writer(csvfile)
            for edge in self.edges:
                writer.writerow(edge)

file_name = input("Enter the filename for the graph: ")  # Changed raw_input to input for Python 3
num_vertices = int(input("Enter the number of vertices: "))
num_edges = int(input("Enter the number of edges (greater than or equal to vertices): "))

if num_edges < num_vertices:
    print("Number of edges must be greater than or equal to the number of vertices.")
else:
    graph = Graph(num_vertices)
    graph.generate_connected_graph()

    # Generate remaining random edges
    additional_edges = num_edges - num_vertices + 1
    while additional_edges > 0:
        v1 = random.randint(0, num_vertices - 1)
        v2 = random.randint(0, num_vertices - 1)
        if v1 != v2:  # Ensure no self-loops
            weight = random.randint(1, 100)
            graph.add_edge(v1, v2, weight)
            additional_edges -= 1

    # Ensure the directory exists before writing
    os.makedirs('inputGraphs', exist_ok=True)  # Create directory if it doesn't exist
    full_path = os.path.join("inputGraphs", file_name + ".csv")
    graph.write_to_csv(full_path)
    print("CSV file created successfully.")


